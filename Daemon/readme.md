# How to setup the daemon

_**Change the path to fit for your system.**_

Copy delila-webapi.service to /etc/systemd/system/

```
sudo cp delila-webapi.service /etc/systemd/system/
```

Start the daemon

```
sudo systemctl start delila-webapi.service
```

If possible, enable the daemon

```
sudo systemctl enable delila-webapi.service
```
