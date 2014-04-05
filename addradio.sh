#! /bin/sh -
# Some configuration settings
VOLUME=60
mpc volume $VOLUME	# adjust this to suit your speakers/amplifier
mpc clear	# clear current playlist

mpc add http://relay3.slayradio.org:8000/						#*Slay Radio
mpc add http://germany-rinse.it.two.dnbradio.com:10128			#*DNBRADIO
mpc add http://shouthost.com.19.streams.bassdrive.com:8200		#*BassDrive 
mpc add http://fr1.streamhosting.ch:8080						#*lounge-radio.com
mpc add http://www.partyvibe.com:8008/							#*chilout
mpc add http://us-tx1.streams.bassdrive.com:9000/				#*DnBHeaven.com
mpc add http://80.94.69.106:6404/ 								#*Liquid DnB DI.FM
mpc add http://91.121.236.242:80/								#*POLSKASTACJA.PL
mpc add http://uk2.internet-radio.com:31491						#*AmbientRadio.org
mpc add http://www.partyvibe.com:8010/							#*PARTY VIBE RADIO
mpc add http://streamer.psyradio.org:8120						#*psyradio chillout
mpc add http://rts.ipradio.rs:8002								#*Radio Belgrad1
mpc add http://rts.ipradio.rs:8004								#*Radio Belgrad2
mpc add http://rts.ipradio.rs:8006								#*Radio Beograd202

#mpc add http://scfire-dtc-aa01.stream.aol.com:80/stream/1046	#*KCRW Simulcast
#mpc add http://208.101.28.234:8004								#*Bassdrive
#mpc add http://205.188.215.232:8016								#*di.fm Soulful House
#mpc add http://scfire-ntc-aa03.stream.aol.com:80/stream/1009	#*di.fm Lounge
#mpc add http://205.188.215.225:8002								#*di.fm Breaks
#mpc add http://scfire-ntc-aa03.stream.aol.com:80/stream/1025	#*di.fm Electro House
#mpc add http://di-fm-01.quintex.com:8888						#*di.fm NYC Exposure
#mpc add http://208.122.59.30:7234								#*di.fm Future Synthpop
#mpc add http://scfire-mtc-aa03.stream.aol.com:80/stream/1026	#*di.fm Progressive
