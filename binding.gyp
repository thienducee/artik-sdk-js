{
  'targets': [
    {
      'target_name': 'artik-sdk',
      'include_dirs': [
                        '/usr/include/artik/base',
                        '/usr/include/artik/base/cpp',
                        '/usr/include/artik/bluetooth',
                        '/usr/include/artik/bluetooth/cpp',
                        '/usr/include/artik/connectivity',
                        '/usr/include/artik/connectivity/cpp',
                        '/usr/include/artik/media',
                        '/usr/include/artik/media/cpp',
                        '/usr/include/artik/sensor',
                        '/usr/include/artik/sensor/cpp',
                        '/usr/include/artik/systemio',
                        '/usr/include/artik/systemio/cpp',
                        '/usr/include/artik/wifi',
                        '/usr/include/artik/wifi/cpp',
                        '/usr/include/artik/zigbee',
                        '/usr/include/artik/zigbee/cpp',
                        '/usr/include/artik/lwm2m',
                        '/usr/include/artik/lwm2m/cpp',
                        '/usr/include/artik/mqtt',
                        '/usr/include/artik/mqtt/cpp',
                        'addon',
                        "<!(node -e \"require('nan')\")"
      ],
      'cflags_cc': [
                '-fexceptions',
                '-DCONFIG_RELEASE',
                '<!@(pkg-config --cflags-only-I glib-2.0)'
      ],
      'link_settings' : {
                'ldflags': ['-Wl,--no-as-needed'],
                'libraries': [
                        '<!@(pkg-config --libs-only-l libartik-sdk-base)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-systemio)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-connectivity)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-media)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-sensor)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-bluetooth)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-wifi)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-zigbee)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-lwm2m)',
                        '<!@(pkg-config --libs-only-l libartik-sdk-mqtt)'
                ]
      },
      'sources': [
        'addon/artik.cc',
        'addon/utils.cc',
        'addon/loop/loop.cc',
        'addon/gpio/gpio.cc',
        'addon/serial/serial.cc',
        'addon/i2c/i2c.cc',
        'addon/pwm/pwm.cc',
        'addon/adc/adc.cc',
        'addon/http/http.cc',
        'addon/websocket/websocket.cc',
        'addon/cloud/cloud.cc',
        'addon/wifi/wifi.cc',
        'addon/media/media.cc',
        'addon/spi/spi.cc',
        'addon/time/time.cc',
        'addon/sensor/sensor.cc',
        'addon/network/network.cc',
        'addon/bluetooth/bluetooth.cc',
        'addon/bluetooth/agent.cc',
        'addon/bluetooth/avrcp.cc',
        'addon/bluetooth/ftp.cc',
        'addon/bluetooth/gatt_client.cc',
        'addon/bluetooth/gatt_server.cc',
        'addon/bluetooth/pan.cc',
        'addon/bluetooth/spp.cc',
        'addon/zigbee/zigbee.cc',
        'addon/zigbee/zigbee_util.cc',
        'addon/zigbee/zigbee_device.cc',
        'addon/lwm2m/lwm2m.cc',
        'addon/mqtt/mqtt.cc',
        'addon/security/security.cc'
      ],
    }
  ]
}

