# cwa-qr-code-generator
This is a simple program which is intended to be used to generate event qr codes for the [corona-warn-app]() automatically.
Its intended use is for permanent facilites. They can install this program on an embedded device (like a Raspberry pi) and it will automatically regerate a valid qr code once a day.

All configuration (location descripton, address, ...) is loaded from a config file which will be place in the boot parition of the pi. An example can be found [here](https://github.com/CodeCrafter912/cwa-qr-code-generator/blob/main/cwa-qr.conf.example).

# Building
To build this, you need to have Qt Creator with Qt 5.15.2 and protobuf installed on your system. You can then use the Qt creaor for building.

# Screenshots
<div>
  <img src="https://github.com/CodeCrafter912/cwa-qr-code-generator/blob/main/resources/screenshots/portrait.png" alt="portrait" width="30%" />
  <img src="https://github.com/CodeCrafter912/cwa-qr-code-generator/blob/main/resources/screenshots/landscape.png" alt="landscape" width="60%" />
</div>

# Used libraries
- [QZXing](https://github.com/ftylitak/qzxing)
- [protobuf](https://github.com/protocolbuffers/protobuf)
