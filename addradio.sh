#! /bin/sh -
# Some configuration settings
VOLUME=60
mpc volume $VOLUME	# adjust this to suit your speakers/amplifier
mpc clear	# clear current playlist

mpc add http://relay3.slayradio.org:8000/						#*Slay Radio
mpc add http://scfire-dtc-aa01.stream.aol.com:80/stream/1046	#*KCRW Simulcast
mpc add http://208.101.28.234:8004								#*Bassdrive
mpc add http://205.188.215.232:8016								#*di.fm Soulful House
mpc add http://scfire-ntc-aa03.stream.aol.com:80/stream/1009	#*di.fm Lounge
mpc add http://205.188.215.225:8002								#*di.fm Breaks
mpc add http://scfire-ntc-aa03.stream.aol.com:80/stream/1025	#*di.fm Electro House
mpc add http://di-fm-01.quintex.com:8888						#*di.fm NYC Exposure
mpc add http://208.122.59.30:7234								#*di.fm Future Synthpop
mpc add http://scfire-mtc-aa03.stream.aol.com:80/stream/1026	#*di.fm Progressive
