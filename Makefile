setup: 
	npm install -g particle-cli

login:
	particle login

build:
	particle compile photon ./firmware/ --saveTo photon_firmware.bin

flash:
	particle flash ${TVBACKLIGHT_DEVICE_ID} photon_firmware.bin