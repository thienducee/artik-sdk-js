var Zigbee = require('../src/zigbee').Zigbee
var Switch = require('../src/zigbee').ZigbeeDevices.ONOFF_SWITCH

var options = {
  withReset: false,
  mode: 'default'
}

process.argv.forEach(function (opt) {
  if (opt === '--reset') {
    options.withReset = true
  } else if (opt === 'ezmode') {
    options.mode = 'ezmode'
  }
})

var znode = new Zigbee()
var devSwitch = new Switch(znode, 1)

znode.on('network_notification', function (event) {
  switch (event.status) {
    case 'find_form':
      /* Coordinator: Form success. */
      /* Allow to join this network during 3 minutes (60secs * 3) */
      console.log('form success. permitjoin(3 minutes)')
      znode.network_permitjoin(60 * 3)
      break
    case 'find_form_failed':
      /* Coordinator: Form failed. */
      /* Leave from current network and retry */
      console.log('form failed')
      znode.network_leave()
      znode.network_form()
      break
    case 'join':
      /* Coordinator: Client joined */
      console.log('new router/end-node joined')
      znode.device_discover()
      break
    case 'find_join':
      /* Router: Join success */
      console.log('join success')
      znode.device_discover()
      break
    case 'find_join_failed':
      /* Router: Join failed. */
      /* Change role to Coordinator */
      console.log('join failed')
      znode.network_form()
      break
    default:
      break
  }
})

znode.on('device_discover', function (event) {
  /* Device discovered */
  if (event.status === 'found') {
    console.log('device found:', event.device)
    event.device.endpoints.forEach(function (ep) {
      /* Send command: 'on', 'off' or 'toggle' */
      devSwitch.onoff_command(ep, 'toggle')
    })
  } else {
    console.log('device discover status:', event.status)
  }
})

znode.on('commissioning_status', function (event) {
  console.log('ezmode commissioning_status:', event.status)
})

znode.initialize()
znode.network_start()

if (options.withReset) {
  /* Reset network status (leave from network & cleanup bind table) */
  console.log('reset')
  znode.reset_local()
}

var status = znode.network_request_my_network_status()
console.log('- network_status =', status)
console.log('- device_node_type =', znode.device_request_my_node_type())

if (options.mode === 'ezmode') {
  /* EZMode commissioning - automatic join/form */
  devSwitch.ezmode_commissioning_initiator_start()
} else if (options.mode === 'default') {
  if (status === 'joined_network' || status === 'joining_network') {
    /* Network is already configured - Discover devices */
    znode.device_discover()
  } else {
     /* Try to join the existing network (Router role) */
     /* Coordinator should permit(network_permitjoin(N-secs)) this joining. */
    znode.network_join()
  }
}
