#!/usr/bin/python
# coding: UTF-8

# kegdata service to read about key status
# Written by: Ron Ritchey
from __future__ import unicode_literals

import json, threading, logging, Queue, time, getopt, sys, logging
import RPi.GPIO as GPIO
import pymysql.cursors
from hx711 import HX711



# HOW TO CALCULATE THE REFFERENCE UNIT
# To set the reference unit to 1. Put 1kg on your sensor or anything you have and know exactly how much it weights.
# In this case, 92 is 1 gram because, with 1 as a reference unit I got numbers near 0 without any weight
# and I got numbers around 184000 when I added 2kg. So, according to the rule of thirds:
# If 2000 grams is 184000 then 1000 grams is 184000 / 2000 = 92.
#hx.set_reference_unit(1)
#hx.set_reference_unit(92)
#hx.set_reference_unit(10772)


class kegdata():


	kegdata_init = {
		'name':'',
		'description':'',
		'abv':0.0,
		'ibu':0.0,
		'originalgravity':0.0,
		'finalgravity':0.0,
		'color':0.0,
		'kegged':datetime.datetime.fromtimestamp(0),
		'tapped':datetime.datetime.fromtimestamp(0),
		'brewed':datetime.datetime.fromtimestamp(0),
		'notes':'',
		'weight':0.0,
	}
	varcheck = {
		u'unicode':
		[
			u'name',
			u'description',
			u'notes'
		],
		u'int':
		[
			u'weight',
		],
		u'float':
		[
			u'abv',
			u'ibu',
			u'originalgravity',
			u'finalgravity',
			u'color'
		],
		u'datetime.datetime':
		[
			u'kegged',
			u'tapped',
			u'brewed',
		]
	}


	def __init__(self, q, server, port, pwd, tap):
		self.dataqueue = q
		self.kegdata = self.kegdata_init
		self.kegdata_prev = { }

		print "Initializing keg data service"

		self.hx = HX711(4,17)
		self.hx.set_reading_format("LSB", "MSB")
		self.hx.set_reference_unit(673)
		self.hx.reset()
		self.hx.tare()

		# Now set up a thread to listen to the channel and update our data when
		# the channel indicates a relevant key has changed
		data_t = threading.Thread(target=self.run)
		data_t.daemon = True
		data_t.start()


		self.server = server
		self.port = port
		self.pwd = pwd
		self.tap = tap
		self.connection_failed = 0

		self.dataclient = None


	def validatekegvars(self, vars):

		for vtype, members in self.varcheck.iteritems():

			if vtype == u'unicode':
				for v in members:
					try:
						if type(vars[v]) is unicode:
							continue
						if type(vars[v]) is None:
							vars[v] = u""
						elif type(vars[v]) is str:
							logging.debug(u"Received string in {0}.  Converting to Unicode".format(v))
							vars[v] = vars[v].decode()
						else:
							# This happens so often when playing from webradio that I'm disabling logging for now.
#							logging.debug(u"Received non-string type {0} in {1}.  Converting to null".format(type(vars[v]),v))
							vars[v] = u""
					except KeyError:
						logging.debug(u"Missing required value {0}.  Adding empty version".format(v))
						vars[v] = u""
			elif vtype == u'bool':
				for v in members:
					try:
						if type(vars[v]) is bool:
							continue
						if type(vars[v]) is None:
							vars[v] = False
						elif type(vars[v]) is int:
							logging.debug(u"Received integer in {0}.  Converting to boolean".format(v))
							vars[v] = bool(vars[v])
						else:
							logging.debug(u"Received non-bool type {0} in {1}.  Converting to False".format(type(vars[v]),v))
							vars[v] = False
					except KeyError:
						logging.debug(u"Missing required value {0}.  Adding empty version".format(v))
						vars[v] = False
			elif vtype == u'int':
				for v in members:
					try:
						if type(vars[v]) is int:
							continue
						if type(vars[v]) is None:
							vars[v] = 0
						elif type(vars[v]) is bool:
							logging.debug(u"Received boolean in {0}.  Converting to integer".format(v))
							vars[v] = int(vars[v])
						else:
							logging.debug(u"Received non-integer type {0} in {1}.  Converting to 0".format(type(vars[v]),v))
							vars[v] = 0
					except KeyError:
						logging.debug(u"Missing required value {0}.  Adding empty version".format(v))
						vars[v] = 0
			elif vtype == u'datetime.datetime':
				for v in members:
					try:
						if type(vars[v]) is datetime.datetime:
							continue
						if type(vars[v]) is None:
							vars[v] = datetime.datetime.fromtimestamp(0)
						else:
							logging.debug(u"Received non-datetime type {0} in {1}.  Converting to empty date".format(type(vars[v]),v))
							vars[v] = datetime.datetime.fromtimestamp(0)
					except KeyError:
						logging.debug(u"Missing required value {0}.  Adding empty version".format(v))
						vars[v] = datetime.datetime.fromtimestamp(0)


	def connect(self):

		# Try up to 10 times to connect to REDIS
		self.connection_failed = 0

		logging.debug(u"Connecting to mySQL service on {0}:{1}".format(self.server, self.port))

		while True:
			if self.connection_failed >= 10:
				logging.debug(u"Could not connect to mySQL service")
				raise RuntimeError(u"Could not connect to mySQL service")
			try:
				# Connection to mySQL
				conn = pymysql.connect(host=self.server,
					db='pydKeg',
					user='root',
					charset='utf8',
					password=self.pwd,
					cursorclass=pymysql.cursors.DictCursor)

				if conn.open:
					logging.debug('Connected to MySQL database')

				self.dataclient = conn
				break
			except:
				self.dataclient = None
				self.connection_failed += 1
				time.sleep(1)

	#
	# def subscribe(self):
	# 	# Try to subscribe.  If you fail, reconnect and try again.
	# 	# If you fail, allow the resulting exception to be passed on.
	#
	# 	try:
	# 		# Create a pubsub to receive messages
	# 		self.pubsub = self.dataclient.pubsub(ignore_subscribe_messages=True)
	#
	# 		# Subscribe to act_player_info keyspace events
	# 		self.pubsub.psubscribe(u'__key*__:act_player_info')
	# 	except redis.ConnectionError:
	# 		self.connect()
	#
	# 		# Try again to subscribe
	# 		# Create a pubsub to receive messages
	# 		self.pubsub = self.dataclient.pubsub(ignore_subscribe_messages=True)
	#
	# 		# Subscribe to act_player_info keyspace events
	# 		self.pubsub.subscribe(u'__key*__:act_player_info')

	def run(self):

		logging.debug(u"kegdata service starting")

		while True:
			# if self.dataclient is None:
			# 	try:
			# 		# Try to connect
			# 		self.connect()
			# 		self.subscribe()
			# 		self.status()
			# 		self.sendUpdate()
			# 	except (redis.ConnectionError, RuntimeError):
			# 		self.dataclient = None
			# 		# On connection error, sleep 5 and then return to top and try again
			# 		time.sleep(5)
			# 		continue
			# try:
			# 	# Wait for notice that key has changed
			# 	msg = self.pubsub.get_message()
			# 	if msg:
			# 		# act_player_info key event occured
			# 		self.status()
			# 		self.sendUpdate()
			# 	time.sleep(.01)
			# except (redis.ConnectionError, RuntimeError):
			# 	# if we lose our connection while trying to query DB
			# 	# sleep 5 and then return to top to try again
			# 	self.dataclient = None
			# 	logging.debug(u"Could not get status from Rune Redis service")
			# 	time.sleep(5)
			# 	continue
			self.status()
			self.sendUpdate()
			time.sleep(5)


	def status(self):
		# Read kegplayer status and update kegdata

		cursor = self.dataclient.cursor()
		cursor.execute("SELECT Beer FROM Tap WHERE Tap.Tap = %s", (self.tap) )
		row = cursor.fetchone()
		if row is not None:
			Beer = row['Beer']
		else:
			Beer = ''

		cursor.execute("SELECT * FROM Beer WHERE Beer.idBeer = %s", (Beer))
		row = cursor.fetchone()
		if row is not None:
			# Update keg variables
			self.kegdata[u'name'] = row['Name'] if 'Name' in row else u''
			self.kegdata[u'description'] = row['Description'] if 'Description' in row else u''
			self.kegdata[u'abv'] = row['ABV'] if 'ABV' in row else 0.0
			self.kegdata[u'ibu'] = row['IBU'] if 'IBU' in row else 0.0
			self.kegdata[u'originalgravity'] = row['OriginalGravity'] if 'OriginalGravity' in row else 0.0
			self.kegdata[u'finalgravity'] = row['FinalGravity'] if 'FinalGravity' in row else 0.0
			self.kegdata[u'color'] = row['Color'] if 'Color' in row else 0.0
			self.kegdata[u'kegged'] = row['Kegged'] if 'Kegged' in row else datetime.datetime.fromtimestamp(0)
			self.kegdata[u'tapped'] = row['Tapped'] if 'Tapped' in row else datetime.datetime.fromtimestamp(0)
			self.kegdata[u'brewed'] = row['Tapped'] if 'Tapped' in row else datetime.datetime.fromtimestamp(0)
			self.kegdata[u'notes'] = row['Notes'] if 'Notes' in row else u''
		else:
			self.kegdata[u'name'] = u''
			self.kegdata[u'description'] = u''
			self.kegdata[u'abv'] = 0.0
			self.kegdata[u'ibu'] = 0.0
			self.kegdata[u'originalgravity'] = 0.0
			self.kegdata[u'finalgravity'] = 0.0
			self.kegdata[u'color'] = 0.0
			self.kegdata[u'kegged'] = datetime.datetime.fromtimestamp(0)
			self.kegdata[u'tapped'] = datetime.datetime.fromtimestamp(0)
			self.kegdata[u'brewed'] = datetime.datetime.fromtimestamp(0)
			self.kegdata[u'notes'] = u''




		self.kegdata[u'weight'] = int(self.hx.get_weight(10))
		print "Weight is {0} in oz".format(self.kegdata[u'weight'])
		self.hx.power_down()
		self.hx.power_up()

		self.validatekegvars(self.kegdata)

	def sendUpdate(self):
		# Figure out what has changed and then send just those values across dataqueue
		md = { }
		for k, v in self.kegdata.iteritems():
			pv = self.kegdata_prev[k] if k in self.kegdata_prev else None
			if pv != v:
				md[k] = v


		# Send md to queue if anything has changed
		if len(md) > 0:
			# # elapsed is special as it needs to be sent to guarantee that the timer gets updated correctly.  Even if it hasn't changed, send it anyway
			# md[u'elapsed'] = self.kegdata[u'elapsed']
			self.dataqueue.put(md)

			# Update kegdata_prev
			self.kegdata_prev = self.kegdata.copy()


if __name__ == u'__main__':

	logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s', filename=u'kegdata.log', level=logging.DEBUG)
	logging.getLogger().addHandler(logging.StreamHandler())

	try:
		opts, args = getopt.getopt(sys.argv[1:],u"hs:p:w:t:",[u"server=",u"port=",u"pwd=",u"tap="])
	except getopt.GetoptError:
		print u'kegdata.py -s <server> -p <port> -w <password> -t <tap>'
		sys.exit(2)

	Set defaults
	server = u'localhost'
	port = 3306
	pwd= u''

	for opt, arg in opts:
		if opt == u'-h':
			print u'kegdata.py -s <server> -p <port> -w <password>'
			sys.exit()
		elif opt in (u"-s", u"--server"):
			server = arg
		elif opt in (u"-p", u"--port"):
			port = arg
		elif opt in (u"-w", u"--pwd"):
			pwd = arg
		elif opt in (u"-t", u"--tap"):
			pwd = int(arg)


	import sys
	q = Queue.Queue()
	kd = kegdata(q. server, port, pwd, tap)

	try:
		start = time.time()
		while True:
			if start+120 < time.time():
				break;
			try:
				item = q.get(timeout=1000)
				print u"++++++++++"
				for k,v in item.iteritems():
					print u"[{0}] '{1}' type {2}".format(k,v,type(v))
				print u"++++++++++"
				print
				q.task_done()
			except Queue.Empty:
				pass
	except KeyboardInterrupt:
		print u''
		pass

	print u"Exiting..."
