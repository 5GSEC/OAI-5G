#/*
# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this file
# * except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *      http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *      contact@openairinterface.org
# */
#---------------------------------------------------------------------
# Python for CI of OAI-eNB + COTS-UE
#
#   Required Python Version
#     Python 3.x
#
#   Required Python Package
#     pexpect
#---------------------------------------------------------------------

#-----------------------------------------------------------
# Import
#-----------------------------------------------------------
import sys	      # arg
import re	       # reg
import logging
import os
import time
import signal

from multiprocessing import Process, Lock, SimpleQueue

#-----------------------------------------------------------
# OAI Testing modules
#-----------------------------------------------------------
import sshconnection as SSH
import helpreadme as HELP
import constants as CONST
import cls_cluster as OC
import cls_cmd
#-----------------------------------------------------------
# Class Declaration
#-----------------------------------------------------------


class EPCManagement():

	def __init__(self):

		self.IPAddress = ''
		self.UserName = ''
		self.Password = ''
		self.SourceCodePath = ''
		self.Type = ''
		self.PcapFileName = ''
		self.testCase_id = ''
		self.MmeIPAddress = ''
		self.containerPrefix = 'prod'
		self.mmeConfFile = 'mme.conf'
		self.yamlPath = ''
		self.isMagmaUsed = False
		self.cfgDeploy = '--type start-mini --scenario 1 --capture /tmp/oai-cn5g-v1.5.pcap' #from xml, 'mini' is default normal for docker-network.py
		self.cfgUnDeploy = '--type stop-mini --scenario 1' #from xml, 'mini' is default normal for docker-network.py
		self.OCUrl = "https://api.oai.cs.eurecom.fr:6443"
		self.OCRegistry = "default-route-openshift-image-registry.apps.oai.cs.eurecom.fr/"
		self.OCUserName = ''
		self.OCPassword = ''
		self.imageToPull = ''
		self.eNBSourceCodePath = ''

#-----------------------------------------------------------
# EPC management functions
#-----------------------------------------------------------

	def InitializeHSS(self, HTML):
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			HELP.GenericHelp(CONST.Version)
			HELP.EPCSrvHelp(self.IPAddress, self.UserName, self.Password, self.SourceCodePath, self.Type)
			sys.exit('Insufficient EPC Parameters')
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 Cassandra-based HSS in Docker')
			mySSH.command('if [ -d ' + self.SourceCodePath + '/scripts ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/scripts ; fi', '\$', 5)
			mySSH.command('mkdir -p ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-hss /bin/bash -c "nohup tshark -i eth0 -i eth1 -w /tmp/hss_check_run.pcap 2>&1 > /dev/null"', '\$', 5)
			time.sleep(5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-hss /bin/bash -c "nohup ./bin/oai_hss -j ./etc/hss_rel14.json --reloadkey true > hss_check_run.log 2>&1"', '\$', 5)
		elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 Cassandra-based HSS')
			mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
			logging.debug('\u001B[1m Launching tshark on all interfaces \u001B[0m')
			self.PcapFileName = 'epc_' + self.testCase_id + '.pcap'
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f ' + self.PcapFileName, '\$', 5)
			mySSH.command('echo $USER; nohup sudo tshark -f "tcp port not 22 and port not 53" -i any -w ' + self.SourceCodePath + '/scripts/' + self.PcapFileName + ' > /tmp/tshark.log 2>&1 &', self.UserName, 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S mkdir -p logs', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f hss_' + self.testCase_id + '.log logs/hss*.*', '\$', 5)
			mySSH.command('echo "oai_hss -j /usr/local/etc/oai/hss_rel14.json" > ./my-hss.sh', '\$', 5)
			mySSH.command('chmod 755 ./my-hss.sh', '\$', 5)
			mySSH.command('sudo daemon --unsafe --name=hss_daemon --chdir=' + self.SourceCodePath + '/scripts -o ' + self.SourceCodePath + '/scripts/hss_' + self.testCase_id + '.log ./my-hss.sh', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC HSS')
			mySSH.command('cd ' + self.SourceCodePath, '\$', 5)
			mySSH.command('source oaienv', '\$', 5)
			mySSH.command('cd scripts', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./run_hss 2>&1 | stdbuf -o0 awk \'{ print strftime("[%Y/%m/%d %H:%M:%S] ",systime()) $0 }\' | stdbuf -o0 tee -a hss_' + self.testCase_id + '.log &', 'Core state: 2 -> 3', 35)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			logging.debug('Using the ltebox simulated HSS')
			mySSH.command('if [ -d ' + self.SourceCodePath + '/scripts ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/scripts ; fi', '\$', 5)
			mySSH.command('mkdir -p ' + self.SourceCodePath + '/scripts', '\$', 5)
			result = re.search('hss_sim s6as diam_hss', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall hss_sim', '\$', 5)
			mySSH.command('ps aux | grep --colour=never xGw | grep -v grep', '\$', 5, silent=True)
			result = re.search('root.*xGw', mySSH.getBefore())
			if result is not None:
				mySSH.command('cd /opt/ltebox/tools', '\$', 5)
				mySSH.command('echo ' + self.Password + ' | sudo -S ./stop_ltebox', '\$', 5)
			mySSH.command('cd /opt/hss_sim0609', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f hss.log', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S echo "Starting sudo session" && sudo su -c "screen -dm -S simulated_hss ./starthss"', '\$', 5)
		else:
			logging.error('This option should not occur!')
		mySSH.close()
		HTML.CreateHtmlTestRow(self.Type, 'OK', CONST.ALL_PROCESSES_OK)

	def InitializeMME(self, HTML):
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			HELP.GenericHelp(CONST.Version)
			HELP.EPCSrvHelp(self.IPAddress, self.UserName, self.Password, self.SourceCodePath, self.Type)
			sys.exit('Insufficient EPC Parameters')
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 MME in Docker')
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-mme /bin/bash -c "nohup tshark -i eth0 -i lo:s10 -f "not port 2152" -w /tmp/mme_check_run.pcap 2>&1 > /dev/null"', '\$', 5)
			time.sleep(5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-mme /bin/bash -c "nohup ./bin/oai_mme -c ./etc/' + self.mmeConfFile + ' > mme_check_run.log 2>&1"', '\$', 5)
		elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 MME')
			mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f mme_' + self.testCase_id + '.log', '\$', 5)
			mySSH.command('echo "./run_mme --config-file /usr/local/etc/oai/mme.conf --set-virt-if" > ./my-mme.sh', '\$', 5)
			mySSH.command('chmod 755 ./my-mme.sh', '\$', 5)
			mySSH.command('sudo daemon --unsafe --name=mme_daemon --chdir=' + self.SourceCodePath + '/scripts -o ' + self.SourceCodePath + '/scripts/mme_' + self.testCase_id + '.log ./my-mme.sh', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath, '\$', 5)
			mySSH.command('source oaienv', '\$', 5)
			mySSH.command('cd scripts', '\$', 5)
			mySSH.command('stdbuf -o0 hostname', '\$', 5)
			result = re.search('hostname\\\\r\\\\n(?P<host_name>[a-zA-Z0-9\-\_]+)\\\\r\\\\n', mySSH.getBefore())
			if result is None:
				logging.debug('\u001B[1;37;41m Hostname Not Found! \u001B[0m')
				sys.exit(1)
			host_name = result.group('host_name')
			mySSH.command('echo ' + self.Password + ' | sudo -S ./run_mme 2>&1 | stdbuf -o0 tee -a mme_' + self.testCase_id + '.log &', 'MME app initialization complete', 100)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			# Clean-up the logs from previous runs
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f ../var/log/*.0', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./start_mme', '\$', 5)
		else:
			logging.error('This option should not occur!')
		mySSH.close()
		HTML.CreateHtmlTestRow(self.Type, 'OK', CONST.ALL_PROCESSES_OK)

	def SetMmeIPAddress(self):
		# Not an error if we don't need an EPC
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			return
		if self.IPAddress == 'none':
			return
		# Only in case of Docker containers, MME IP address is not the EPC HOST IP address
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			self.isMagmaUsed = False
			mySSH.command('docker ps -a', '\$', 5)
			result = re.search('magma', mySSH.getBefore())
			if result is not None:
				self.isMagmaUsed = True
			if self.isMagmaUsed:
				mySSH.command('docker inspect --format="MME_IP_ADDR = {{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}" ' + self.containerPrefix + '-magma-mme', '\$', 5)
			else:
				mySSH.command('docker inspect --format="MME_IP_ADDR = {{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}" ' + self.containerPrefix + '-oai-mme', '\$', 5)
			result = re.search('MME_IP_ADDR = (?P<mme_ip_addr>[0-9\.]+)', mySSH.getBefore())
			if result is not None:
				self.MmeIPAddress = result.group('mme_ip_addr')
				logging.debug('MME IP Address is ' + self.MmeIPAddress)
			mySSH.close()
		else:
			self.MmeIPAddress = self.IPAddress

	def InitializeSPGW(self, HTML):
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			HELP.GenericHelp(CONST.Version)
			HELP.EPCSrvHelp(self.IPAddress, self.UserName, self.Password, self.SourceCodePath, self.Type)
			sys.exit('Insufficient EPC Parameters')
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 SPGW-CUPS in Docker')
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-spgwc /bin/bash -c "nohup tshark -i eth0 -i lo:p5c -i lo:s5c -f "not port 2152" -w /tmp/spgwc_check_run.pcap 2>&1 > /dev/null"', '\$', 5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-spgwu-tiny /bin/bash -c "nohup tshark -i eth0 -f "not port 2152" -w /tmp/spgwu_check_run.pcap 2>&1 > /dev/null"', '\$', 5)
			time.sleep(5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-spgwc /bin/bash -c "nohup ./bin/oai_spgwc -o -c ./etc/spgw_c.conf > spgwc_check_run.log 2>&1"', '\$', 5)
			time.sleep(5)
			mySSH.command('docker exec -d ' + self.containerPrefix + '-oai-spgwu-tiny /bin/bash -c "nohup ./bin/oai_spgwu -o -c ./etc/spgw_u.conf > spgwu_check_run.log 2>&1"', '\$', 5)
		elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			logging.debug('Using the OAI EPC Release 14 SPGW-CUPS')
			mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f spgwc_' + self.testCase_id + '.log spgwu_' + self.testCase_id + '.log', '\$', 5)
			mySSH.command('echo "spgwc -c /usr/local/etc/oai/spgw_c.conf" > ./my-spgwc.sh', '\$', 5)
			mySSH.command('chmod 755 ./my-spgwc.sh', '\$', 5)
			mySSH.command('sudo daemon --unsafe --name=spgwc_daemon --chdir=' + self.SourceCodePath + '/scripts -o ' + self.SourceCodePath + '/scripts/spgwc_' + self.testCase_id + '.log ./my-spgwc.sh', '\$', 5)
			time.sleep(5)
			mySSH.command('echo "spgwu -c /usr/local/etc/oai/spgw_u.conf" > ./my-spgwu.sh', '\$', 5)
			mySSH.command('chmod 755 ./my-spgwu.sh', '\$', 5)
			mySSH.command('sudo daemon --unsafe --name=spgwu_daemon --chdir=' + self.SourceCodePath + '/scripts -o ' + self.SourceCodePath + '/scripts/spgwu_' + self.testCase_id + '.log ./my-spgwu.sh', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath, '\$', 5)
			mySSH.command('source oaienv', '\$', 5)
			mySSH.command('cd scripts', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./run_spgw 2>&1 | stdbuf -o0 tee -a spgw_' + self.testCase_id + '.log &', 'Initializing SPGW-APP task interface: DONE', 30)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./start_xGw', '\$', 5)
		else:
			logging.error('This option should not occur!')
		mySSH.close()
		HTML.CreateHtmlTestRow(self.Type, 'OK', CONST.ALL_PROCESSES_OK)

	def Initialize5GCN(self, HTML):
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.Type == '':
			HELP.GenericHelp(CONST.Version)
			HELP.EPCSrvHelp(self.IPAddress, self.UserName, self.Password, self.Type)
			sys.exit('Insufficient EPC Parameters')
		mySSH = cls_cmd.getConnection(self.IPAddress)
		html_cell = ''
		if re.match('ltebox', self.Type, re.IGNORECASE):
			logging.debug('Using the SABOX simulated HSS')
			mySSH.command('if [ -d ' + self.SourceCodePath + '/scripts ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/scripts ; fi', '\$', 5)
			mySSH.command('mkdir -p ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('cd /opt/hss_sim0609', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm -f hss.log', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S echo "Starting sudo session" && sudo su -c "screen -dm -S simulated_5g_hss ./start_5g_hss"', '\$', 5)
			logging.debug('Using the sabox')
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./start_sabox', '\$', 5)
			html_cell += 'N/A\n'
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			logging.debug('Starting OAI CN5G')
			mySSH.command('if [ -d ' + self.SourceCodePath + '/scripts ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/scripts ; fi', '\$', 5)
			mySSH.command('mkdir -p ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('cd /opt/oai-cn5g-fed-v1.5/docker-compose', '\$', 5)
			mySSH.command('python3 ./core-network.py '+self.cfgDeploy, '\$', 60)
			if re.search('start-mini-as-ue', self.cfgDeploy):
				dFile = 'docker-compose-mini-nrf-asue.yaml'
			else:
				dFile = 'docker-compose-mini-nrf.yaml'
			mySSH.command('docker-compose -f ' + dFile + ' ps -a', '\$', 60)
			if mySSH.getBefore().count('Up (healthy)') != 6:
				logging.error('Not all container healthy')
			else:
				logging.debug('OK --> all containers are healthy')
			mySSH.command('docker-compose -f ' + dFile + ' config | grep --colour=never image', '\$', 10)
			listOfImages = mySSH.getBefore()
			for imageLine in listOfImages.split('\\r\\n'):
				res1 = re.search('image: (?P<name>[a-zA-Z0-9\-/]+):(?P<tag>[a-zA-Z0-9\-]+)', str(imageLine))
				res2 = re.search('mysql', str(imageLine))
				if res1 is not None and res2 is None:
					html_cell += res1.group('name') + ':' + res1.group('tag') + ' '
					nbChars = len(res1.group('name')) + len(res1.group('tag')) + 2
					while (nbChars < 32):
						html_cell += ' '
						nbChars += 1
					mySSH.command('docker image inspect --format="Size = {{.Size}} bytes" ' + res1.group('name') + ':' + res1.group('tag'), '\$', 10)
					res3 = re.search('Size *= *(?P<size>[0-9\-]*) *bytes', mySSH.getBefore())
					if res3 is not None:
						imageSize = int(res3.group('size'))
						imageSize = int(imageSize/(1024*1024))
						html_cell += str(imageSize) + ' MBytes '
					mySSH.command('docker image inspect --format="Date = {{.Created}}" ' + res1.group('name') + ':' + res1.group('tag'), '\$', 10)
					res4 = re.search('Date *= *(?P<date>[0-9\-]*)T', mySSH.getBefore())
					if res4 is not None:
						html_cell += '(' + res4.group('date') + ')'
					html_cell += '\n'
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			self.testCase_id = HTML.testCase_id
			imageNames = ["oai-nrf", "oai-amf", "oai-smf", "oai-spgwu-tiny", "oai-ausf", "oai-udm", "oai-udr", "mysql","oai-traffic-server"]
			logging.debug('Deploying OAI CN5G on Openshift Cluster')
			lIpAddr = self.IPAddress
			lSourcePath = "/opt/oai-cn5g-fed-develop-2023-04-28-20897"
			succeeded = OC.OC_login(mySSH, self.OCUserName, self.OCPassword, OC.CI_OC_CORE_NAMESPACE)
			if not succeeded:
				logging.error('\u001B[1m OC Cluster Login Failed\u001B[0m')
				HTML.CreateHtmlTestRow('N/A', 'KO', CONST.OC_LOGIN_FAIL)
				return False
			for ii in imageNames:
					mySSH.run(f'helm uninstall {ii}', reportNonZero=False)
			mySSH.run(f'helm spray {lSourcePath}/ci-scripts/charts/oai-5g-basic/.')
			ret = mySSH.run(f'oc get pods', silent=True)
			if ret.stdout.count('Running') != 9:
				logging.error('\u001B[1m Deploying 5GCN Failed using helm chart on OC Cluster\u001B[0m')
				for ii in imageNames:
					mySSH.run('helm uninstall '+ ii)
				ret = mySSH.run(f'oc get pods')
				if re.search('No resources found', ret.stdout):
					logging.debug('All pods uninstalled')
					OC.OC_logout(mySSH)
					mySSH.close()
					HTML.CreateHtmlTestRow('N/A', 'KO', CONST.OC_PROJECT_FAIL)
					return False
			ret = mySSH.run(f'oc get pods', silent=True)
			for line in ret.stdout.split('\n')[1:]:
				columns = line.strip().split()
				name = columns[0]
				status = columns[2]
				html_cell += status + '    ' + name
				html_cell += '\n'
			OC.OC_logout(mySSH)
		else:
			logging.error('This option should not occur!')
		mySSH.close()
		HTML.CreateHtmlTestRowQueue(self.Type, 'OK', [html_cell])

	def SetAmfIPAddress(self):
		# Not an error if we don't need an 5GCN
		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			return
		if self.IPAddress == 'none':
			return
		if re.match('ltebox', self.Type, re.IGNORECASE):
			self.MmeIPAddress = self.IPAddress
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			response=mySSH.command3('docker container ls -f name=oai-amf', 10)
			if len(response)>1:
				response=mySSH.command3('docker inspect --format=\'{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}\' oai-amf', 10)
				tmp = str(response[0],'utf-8')
				self.MmeIPAddress = tmp.rstrip()
				logging.debug('AMF IP Address ' + self.MmeIPAddress)
			else:
				logging.error('no container with name oai-amf found, could not retrieve AMF IP address')
			mySSH.close()
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			response=mySSH.command3('oc pods ls -f name=oai-amf', 10)

	def CheckHSSProcess(self, status_queue):
		try:
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
				mySSH.command('docker top ' + self.containerPrefix + '-oai-hss', '\$', 5)
			else:
				mySSH.command('stdbuf -o0 ps -aux | grep --color=never hss | grep -v grep', '\$', 5)
			if re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE) or re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
				result = re.search('oai_hss -j', mySSH.getBefore())
			elif re.match('OAI', self.Type, re.IGNORECASE):
				result = re.search('\/bin\/bash .\/run_', mySSH.getBefore())
			elif re.match('ltebox', self.Type, re.IGNORECASE):
				result = re.search('hss_sim s6as diam_hss', mySSH.getBefore())
			else:
				logging.error('This should not happen!')
			if result is None:
				logging.debug('\u001B[1;37;41m HSS Process Not Found! \u001B[0m')
				status_queue.put(CONST.HSS_PROCESS_FAILED)
			else:
				status_queue.put(CONST.HSS_PROCESS_OK)
			mySSH.close()
		except:
			os.kill(os.getppid(),signal.SIGUSR1)

	def CheckMMEProcess(self, status_queue):
		try:
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
				self.isMagmaUsed = False
				mySSH.command('docker ps -a', '\$', 5)
				result = re.search('magma', mySSH.getBefore())
				if result is not None:
					self.isMagmaUsed = True
				if self.isMagmaUsed:
					mySSH.command('docker top ' + self.containerPrefix + '-magma-mme', '\$', 5)
				else:
					mySSH.command('docker top ' + self.containerPrefix + '-oai-mme', '\$', 5)
			else:
				mySSH.command('stdbuf -o0 ps -aux | grep --color=never mme | grep -v grep', '\$', 5)
			if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
				result = re.search('oai_mme -c ', mySSH.getBefore())
			elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
				result = re.search('mme -c', mySSH.getBefore())
			elif re.match('OAI', self.Type, re.IGNORECASE):
				result = re.search('\/bin\/bash .\/run_', mySSH.getBefore())
			elif re.match('ltebox', self.Type, re.IGNORECASE):
				result = re.search('mme|amf', mySSH.getBefore())
			else:
				logging.error('This should not happen!')
			if result is None:
				logging.debug('\u001B[1;37;41m MME|AMF Process Not Found! \u001B[0m')
				status_queue.put(CONST.MME_PROCESS_FAILED)
			else:
				status_queue.put(CONST.MME_PROCESS_OK)
			mySSH.close()
		except:
			os.kill(os.getppid(),signal.SIGUSR1)

	def CheckSPGWProcess(self, status_queue):
		try:
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
				mySSH.command('docker top ' + self.containerPrefix + '-oai-spgwc', '\$', 5)
				result = re.search('oai_spgwc -', mySSH.getBefore())
				if result is not None:
					mySSH.command('docker top ' + self.containerPrefix + '-oai-spgwu-tiny', '\$', 5)
					result = re.search('oai_spgwu -', mySSH.getBefore())
			elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
				mySSH.command('stdbuf -o0 ps -aux | grep --color=never spgw | grep -v grep', '\$', 5)
				result = re.search('spgwu -c ', mySSH.getBefore())
			elif re.match('OAI', self.Type, re.IGNORECASE):
				mySSH.command('stdbuf -o0 ps -aux | grep --color=never spgw | grep -v grep', '\$', 5)
				result = re.search('\/bin\/bash .\/run_', mySSH.getBefore())
			elif re.match('ltebox', self.Type, re.IGNORECASE):
				mySSH.command('stdbuf -o0 ps -aux | grep --color=never xGw | grep -v grep', '\$', 5)
				result = re.search('xGw|upf', mySSH.getBefore())
			else:
				logging.error('This should not happen!')
			if result is None:
				logging.debug('\u001B[1;37;41m SPGW|UPF Process Not Found! \u001B[0m')
				status_queue.put(CONST.SPGW_PROCESS_FAILED)
			else:
				status_queue.put(CONST.SPGW_PROCESS_OK)
			mySSH.close()
		except:
			os.kill(os.getppid(),signal.SIGUSR1)

	def TerminateHSS(self, HTML):
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-hss /bin/bash -c "killall --signal SIGINT oai_hss tshark"', '\$', 5)
			time.sleep(2)
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-hss /bin/bash -c "ps aux | grep oai_hss"', '\$', 5)
			result = re.search('oai_hss -j ', mySSH.getBefore())
			if result is not None:
				mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-hss /bin/bash -c "killall --signal SIGKILL oai_hss"', '\$', 5)
		elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT oai_hss || true', '\$', 5)
			time.sleep(2)
			mySSH.command('stdbuf -o0  ps -aux | grep --colour=never hss | grep -v grep', '\$', 5)
			result = re.search('oai_hss -j', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGKILL oai_hss || true', '\$', 5)
			mySSH.command('rm -f ' + self.SourceCodePath + '/scripts/my-hss.sh', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE):
			mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT run_hss oai_hss || true', '\$', 5)
			time.sleep(2)
			mySSH.command('stdbuf -o0  ps -aux | grep --colour=never hss | grep -v grep', '\$', 5)
			result = re.search('\/bin\/bash .\/run_', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGKILL run_hss oai_hss || true', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath, '\$', 5)
			mySSH.command('cd scripts', '\$', 5)
			time.sleep(1)
			mySSH.command('echo ' + self.Password + ' | sudo -S screen -S simulated_hss -X quit', '\$', 5)
			time.sleep(5)
			mySSH.command('ps aux | grep --colour=never hss_sim | grep -v grep', '\$', 5, silent=True)
			result = re.search('hss_sim s6as diam_hss', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall hss_sim', '\$', 5)
		else:
			logging.error('This should not happen!')
		mySSH.close()
		HTML.CreateHtmlTestRow('N/A', 'OK', CONST.ALL_PROCESSES_OK)

	def TerminateMME(self, HTML):
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-mme /bin/bash -c "killall --signal SIGINT oai_mme tshark"', '\$', 5)
			time.sleep(2)
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-mme /bin/bash -c "ps aux | grep oai_mme"', '\$', 5)
			result = re.search('oai_mme -c ', mySSH.getBefore())
			if result is not None:
				mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-mme /bin/bash -c "killall --signal SIGKILL oai_mme"', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE) or re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT run_mme mme || true', '\$', 5)
			time.sleep(2)
			mySSH.command('stdbuf -o0 ps -aux | grep mme | grep -v grep', '\$', 5)
			result = re.search('mme -c', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGKILL run_mme mme || true', '\$', 5)
			mySSH.command('rm -f ' + self.SourceCodePath + '/scripts/my-mme.sh', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./stop_mme', '\$', 5)
			time.sleep(5)
		else:
			logging.error('This should not happen!')
		mySSH.close()
		HTML.CreateHtmlTestRow('N/A', 'OK', CONST.ALL_PROCESSES_OK)

	def TerminateSPGW(self, HTML):
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwc /bin/bash -c "killall --signal SIGINT oai_spgwc tshark"', '\$', 5)
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwu-tiny /bin/bash -c "killall --signal SIGINT oai_spgwu tshark"', '\$', 5)
			time.sleep(2)
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwc /bin/bash -c "ps aux | grep oai_spgwc"', '\$', 5)
			result = re.search('oai_spgwc -o -c ', mySSH.getBefore())
			if result is not None:
				mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwc /bin/bash -c "killall --signal SIGKILL oai_spgwc"', '\$', 5)
			mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwu-tiny /bin/bash -c "ps aux | grep oai_spgwu"', '\$', 5)
			result = re.search('oai_spgwu -o -c ', mySSH.getBefore())
			if result is not None:
				mySSH.command('docker exec -it ' + self.containerPrefix + '-oai-spgwu-tiny /bin/bash -c "killall --signal SIGKILL oai_spgwu"', '\$', 5)
		elif re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT spgwc spgwu || true', '\$', 5)
			time.sleep(2)
			mySSH.command('stdbuf -o0 ps -aux | grep spgw | grep -v grep', '\$', 5)
			result = re.search('spgwc -c |spgwu -c ', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGKILL spgwc spgwu || true', '\$', 5)
			mySSH.command('rm -f ' + self.SourceCodePath + '/scripts/my-spgw*.sh', '\$', 5)
			mySSH.command('stdbuf -o0 ps -aux | grep tshark | grep -v grep', '\$', 5)
			result = re.search('-w ', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT tshark || true', '\$', 5)
				mySSH.command('echo ' + self.Password + ' | sudo -S chmod 666 ' + self.SourceCodePath + '/scripts/*.pcap', '\$', 5)
		elif re.match('OAI', self.Type, re.IGNORECASE):
			mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGINT run_spgw spgw || true', '\$', 5)
			time.sleep(2)
			mySSH.command('stdbuf -o0 ps -aux | grep spgw | grep -v grep', '\$', 5)
			result = re.search('\/bin\/bash .\/run_', mySSH.getBefore())
			if result is not None:
				mySSH.command('echo ' + self.Password + ' | sudo -S killall --signal SIGKILL run_spgw spgw || true', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./stop_xGw', '\$', 5)
		else:
			logging.error('This should not happen!')
		mySSH.close()
		HTML.CreateHtmlTestRow('N/A', 'OK', CONST.ALL_PROCESSES_OK)

	def Terminate5GCN(self, HTML):
		imageNames = ["mysql", "oai-nrf", "oai-amf", "oai-smf", "oai-spgwu-tiny", "oai-ausf", "oai-udm", "oai-udr", "oai-traffic-server"]
		containerInPods = ["", "-c nrf", "-c amf", "-c smf", "-c spgwu", "-c ausf", "-c udm", "-c udr", ""]
		mySSH = cls_cmd.getConnection(self.IPAddress)
		message = ''
		if re.match('ltebox', self.Type, re.IGNORECASE):
			logging.debug('Terminating SA BOX')
			mySSH.command('cd /opt/ltebox/tools', '\$', 5)
			mySSH.command('echo ' + self.Password + ' | sudo -S ./stop_sabox', '\$', 5)
			time.sleep(1)
			mySSH.command('cd ' + self.SourceCodePath, '\$', 5)
			mySSH.command('cd scripts', '\$', 5)
			time.sleep(1)
			mySSH.command('echo ' + self.Password + ' | sudo -S screen -S simulated_5g_hss -X quit', '\$', 5)
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			logging.debug('OAI CN5G Collecting Log files to workspace')
			mySSH.command('echo ' + self.Password + ' | sudo rm -rf ' + self.SourceCodePath + '/logs', '\$', 5)
			mySSH.command('mkdir ' + self.SourceCodePath + '/logs','\$', 5)
			containers_list=['oai-smf','oai-spgwu','oai-amf','oai-nrf']
			for c in containers_list:
				mySSH.command('docker logs ' + c + ' > ' + self.SourceCodePath + '/logs/' + c + '.log', '\$', 5)

			logging.debug('Terminating OAI CN5G')
			mySSH.command('cd /opt/oai-cn5g-fed-v1.5/docker-compose', '\$', 5)
			mySSH.command('python3 ./core-network.py '+self.cfgUnDeploy, '\$', 60)
			mySSH.command('docker volume prune --force || true', '\$', 60)
			time.sleep(2)
			mySSH.command('tshark -r /tmp/oai-cn5g-v1.5.pcap | egrep --colour=never "Tracking area update" ','\$', 30)
			result = re.search('Tracking area update request', mySSH.getBefore())
			if result is not None:
				message = 'UE requested ' + str(mySSH.getBefore().count('Tracking area update request')) + 'Tracking area update request(s)'
			else:
				message = 'No Tracking area update request'
			logging.debug(message)
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			logging.debug('Terminating OAI CN5G on Openshift Cluster')
			lIpAddr = self.IPAddress
			lSourcePath = self.SourceCodePath
			mySSH.run(f'rm -Rf {lSourcePath}/logs')
			mySSH.run(f'mkdir -p {lSourcePath}/logs')
			logging.debug('OC OAI CN5G - Collecting Log files to workspace')
			succeeded = OC.OC_login(mySSH, self.OCUserName, self.OCPassword, OC.CI_OC_CORE_NAMESPACE)
			if not succeeded:
				logging.error('\u001B[1m OC Cluster Login Failed\u001B[0m')
				HTML.CreateHtmlTestRow('N/A', 'KO', CONST.OC_LOGIN_FAIL)
				return False
			mySSH.run(f'oc describe pod &> {lSourcePath}/logs/describe-pods-post-test.log')
			mySSH.run(f'oc get pods.metrics.k8s &> {lSourcePath}/logs/nf-resource-consumption.log')
			for ii, ci in zip(imageNames, containerInPods):
			       podName = mySSH.run(f"oc get pods | grep {ii} | awk \'{{print $1}}\'").stdout.strip()
			       if not podName:
				       logging.debug(f'{ii} pod not found!')
				       HTML.CreateHtmlTestRow(self.Type, 'KO', CONST.INVALID_PARAMETER)
				       HTML.CreateHtmlTabFooter(False)
			       mySSH.run(f'oc logs -f {podName} {ci} &> {lSourcePath}/logs/{ii}.log &')
			       mySSH.run(f'helm uninstall {ii}')
			       podName = ''
			mySSH.run(f'cd {lSourcePath}/logs && zip -r -qq test_logs_CN.zip *.log')
			mySSH.copyin(f'{lSourcePath}/logs/test_logs_CN.zip','test_logs_CN.zip')
			ret = mySSH.run(f'oc get pods', silent=True)
			res = re.search(f'No resources found in {OC.CI_OC_CORE_NAMESPACE} namespace.', ret.stdout)
			if res is not None:
			       logging.debug('OC OAI CN5G components uninstalled')
			       message = 'OC OAI CN5G components uninstalled'
			OC.OC_logout(mySSH)
		else:
			logging.error('This should not happen!')
		mySSH.close()
		HTML.CreateHtmlTestRowQueue(self.Type, 'OK', [message])

	def DeployEpc(self, HTML):
		logging.debug('Trying to deploy')
		if not re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			HTML.CreateHtmlTestRow(self.Type, 'KO', CONST.INVALID_PARAMETER)
			HTML.CreateHtmlTabFooter(False)
			sys.exit('Deploy not possible with this EPC type: ' + self.Type)

		if self.IPAddress == '' or self.UserName == '' or self.Password == '' or self.SourceCodePath == '' or self.Type == '':
			HELP.GenericHelp(CONST.Version)
			HELP.EPCSrvHelp(self.IPAddress, self.UserName, self.Password, self.SourceCodePath, self.Type)
			sys.exit('Insufficient EPC Parameters')
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		mySSH.command('docker-compose --version', '\$', 5)
		result = re.search('docker-compose version 1|Docker Compose version v2', mySSH.getBefore())
		if result is None:
			mySSH.close()
			HTML.CreateHtmlTestRow(self.Type, 'KO', CONST.INVALID_PARAMETER)
			HTML.CreateHtmlTabFooter(False)
			sys.exit('docker-compose not installed on ' + self.IPAddress)

		# Checking if it is a MAGMA deployment
		self.isMagmaUsed = False
		if os.path.isfile('./' + self.yamlPath + '/redis_extern.conf'):
			self.isMagmaUsed = True
			logging.debug('MAGMA MME is used!')

		mySSH.command('if [ -d ' + self.SourceCodePath + '/scripts ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/scripts ; fi', '\$', 5)
		mySSH.command('if [ -d ' + self.SourceCodePath + '/logs ]; then echo ' + self.Password + ' | sudo -S rm -Rf ' + self.SourceCodePath + '/logs ; fi', '\$', 5)
		mySSH.command('mkdir -p ' + self.SourceCodePath + '/scripts ' + self.SourceCodePath + '/logs', '\$', 5)
		mySSH.command('rm -f ' + self.SourceCodePath + '/*.log', '\$', 5)

		# deploying and configuring the cassandra database
		# container names and services are currently hard-coded.
		# they could be recovered by:
		# - docker-compose config --services
		# - docker-compose config | grep container_name
		mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
		mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/docker-compose.yml', self.SourceCodePath + '/scripts')
		if self.isMagmaUsed:
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/entrypoint.sh', self.SourceCodePath + '/scripts')
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/mme.conf', self.SourceCodePath + '/scripts')
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/mme_fd.sprint.conf', self.SourceCodePath + '/scripts')
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/redis_extern.conf', self.SourceCodePath + '/scripts')
			mySSH.command('chmod a+x ' + self.SourceCodePath + '/scripts/entrypoint.sh', '\$', 5)
		else:
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/entrypoint.sh', self.SourceCodePath + '/scripts')
			mySSH.copyout(self.IPAddress, self.UserName, self.Password, './' + self.yamlPath + '/mme.conf', self.SourceCodePath + '/scripts')
			mySSH.command('chmod 775 entrypoint.sh', '\$', 60)
		mySSH.command('wget --quiet --tries=3 --retry-connrefused https://raw.githubusercontent.com/OPENAIRINTERFACE/openair-hss/develop/src/hss_rel14/db/oai_db.cql', '\$', 30)
		mySSH.command('docker-compose down', '\$', 60)
		mySSH.command('docker-compose up -d db_init', '\$', 60)
		# databases take time...
		time.sleep(10)
		cnt = 0
		db_init_status = False
		while (cnt < 10):
			mySSH.command('docker logs prod-db-init', '\$', 5)
			result = re.search('OK', mySSH.getBefore())
			if result is not None:
				cnt = 10
				db_init_status = True
			else:
				time.sleep(5)
				cnt += 1
		mySSH.command('docker rm -f prod-db-init', '\$', 5)
		if not db_init_status:
			HTML.CreateHtmlTestRow(self.Type, 'KO', CONST.INVALID_PARAMETER)
			HTML.CreateHtmlTabFooter(False)
			sys.exit('Cassandra DB deployment/configuration went wrong!')

		# deploying EPC cNFs
		mySSH.command('docker-compose up -d oai_spgwu', '\$', 60)
		if self.isMagmaUsed:
			listOfContainers = 'prod-cassandra prod-oai-hss prod-magma-mme prod-oai-spgwc prod-oai-spgwu-tiny prod-redis'
			expectedHealthyContainers = 6
		else:
			listOfContainers = 'prod-cassandra prod-oai-hss prod-oai-mme prod-oai-spgwc prod-oai-spgwu-tiny'
			expectedHealthyContainers = 5

		# Checking for additional services
		mySSH.command('docker-compose config', '\$', 5)
		configResponse = mySSH.getBefore()
		if configResponse.count('trf_gen') == 1:
			mySSH.command('docker-compose up -d trf_gen', '\$', 60)
			listOfContainers += ' prod-trf-gen'
			expectedHealthyContainers += 1

		mySSH.command('docker-compose config | grep --colour=never image', '\$', 10)
		html_cell = ''
		listOfImages = mySSH.getBefore()
		for imageLine in listOfImages.split('\\r\\n'):
			res1 = re.search('image: (?P<name>[a-zA-Z0-9\-]+):(?P<tag>[a-zA-Z0-9\-]+)', str(imageLine))
			res2 = re.search('cassandra|redis', str(imageLine))
			if res1 is not None and res2 is None:
				html_cell += res1.group('name') + ':' + res1.group('tag') + ' '
				nbChars = len(res1.group('name')) + len(res1.group('tag')) + 2
				while (nbChars < 32):
					html_cell += ' '
					nbChars += 1
				mySSH.command('docker image inspect --format="Size = {{.Size}} bytes" ' + res1.group('name') + ':' + res1.group('tag'), '\$', 10)
				res3 = re.search('Size *= *(?P<size>[0-9\-]*) *bytes', mySSH.getBefore())
				if res3 is not None:
					imageSize = int(res3.group('size'))
					imageSize = int(imageSize/(1024*1024))
					html_cell += str(imageSize) + ' MBytes '
				mySSH.command('docker image inspect --format="Date = {{.Created}}" ' + res1.group('name') + ':' + res1.group('tag'), '\$', 10)
				res4 = re.search('Date *= *(?P<date>[0-9\-]*)T', mySSH.getBefore())
				if res4 is not None:
					html_cell += '(' + res4.group('date') + ')'
				html_cell += '\n'
		# Checking if all are healthy
		cnt = 0
		while (cnt < 3):
			mySSH.command('docker inspect --format=\'{{.State.Health.Status}}\' ' + listOfContainers, '\$', 10)
			unhealthyNb = mySSH.getBefore().count('unhealthy')
			healthyNb = mySSH.getBefore().count('healthy') - unhealthyNb
			startingNb = mySSH.getBefore().count('starting')
			if healthyNb == expectedHealthyContainers:
				cnt = 10
			else:
				time.sleep(10)
				cnt += 1
		logging.debug(' -- ' + str(healthyNb) + ' healthy container(s)')
		logging.debug(' -- ' + str(unhealthyNb) + ' unhealthy container(s)')
		logging.debug(' -- ' + str(startingNb) + ' still starting container(s)')
		if healthyNb == expectedHealthyContainers:
			mySSH.command('docker exec -d prod-oai-hss /bin/bash -c "nohup tshark -i any -f \'port 9042 or port 3868\' -w /tmp/hss_check_run.pcap 2>&1 > /dev/null"', '\$', 5)
			if self.isMagmaUsed:
				mySSH.command('docker exec -d prod-magma-mme /bin/bash -c "nohup tshark -i any -f \'port 3868 or port 2123 or port 36412\' -w /tmp/mme_check_run.pcap 2>&1 > /dev/null"', '\$', 10)
			else:
				mySSH.command('docker exec -d prod-oai-mme /bin/bash -c "nohup tshark -i any -f \'port 3868 or port 2123 or port 36412\' -w /tmp/mme_check_run.pcap 2>&1 > /dev/null"', '\$', 10)
			mySSH.command('docker exec -d prod-oai-spgwc /bin/bash -c "nohup tshark -i any -f \'port 2123 or port 8805\' -w /tmp/spgwc_check_run.pcap 2>&1 > /dev/null"', '\$', 10)
			# on SPGW-U, not capturing on SGI to avoid huge file
			mySSH.command('docker exec -d prod-oai-spgwu-tiny /bin/bash -c "nohup tshark -i any -f \'port 8805\'  -w /tmp/spgwu_check_run.pcap 2>&1 > /dev/null"', '\$', 10)
			mySSH.close()
			logging.debug('Deployment OK')
			HTML.CreateHtmlTestRowQueue(self.Type, 'OK', [html_cell])
		else:
			mySSH.close()
			logging.debug('Deployment went wrong')
			HTML.CreateHtmlTestRowQueue(self.Type, 'KO', [html_cell])

	def UndeployEpc(self, HTML):
		logging.debug('Trying to undeploy')
		# No check down, we suppose everything done before.

		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		# Checking if it is a MAGMA deployment.
		mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
		mySSH.command('docker-compose ps -a', '\$', 5)
		self.isMagmaUsed = False
		result = re.search('magma', mySSH.getBefore())
		if result is not None:
			self.isMagmaUsed = True
			logging.debug('MAGMA MME is used!')
		# Recovering logs and pcap files
		mySSH.command('cd ' + self.SourceCodePath + '/logs', '\$', 5)
		mySSH.command('docker exec -it prod-oai-hss /bin/bash -c "killall --signal SIGINT oai_hss tshark"', '\$', 5)
		if self.isMagmaUsed:
			mySSH.command('docker exec -it prod-magma-mme /bin/bash -c "killall --signal SIGINT tshark"', '\$', 5)
		else:
			mySSH.command('docker exec -it prod-oai-mme /bin/bash -c "killall --signal SIGINT tshark"', '\$', 5)
		mySSH.command('docker exec -it prod-oai-spgwc /bin/bash -c "killall --signal SIGINT oai_spgwc tshark"', '\$', 5)
		mySSH.command('docker exec -it prod-oai-spgwu-tiny /bin/bash -c "killall --signal SIGINT tshark"', '\$', 5)
		mySSH.command('docker logs prod-oai-hss > hss_' + self.testCase_id + '.log', '\$', 5)
		if self.isMagmaUsed:
			mySSH.command('docker cp --follow-link prod-magma-mme:/var/log/mme.log mme_' + self.testCase_id + '.log', '\$', 15)
		else:
			mySSH.command('docker logs prod-oai-mme > mme_' + self.testCase_id + '.log', '\$', 5)
		mySSH.command('docker logs prod-oai-spgwc > spgwc_' + self.testCase_id + '.log', '\$', 5)
		mySSH.command('docker logs prod-oai-spgwu-tiny > spgwu_' + self.testCase_id + '.log', '\$', 5)
		mySSH.command('docker cp prod-oai-hss:/tmp/hss_check_run.pcap hss_' + self.testCase_id + '.pcap', '\$', 60)
		if self.isMagmaUsed:
			mySSH.command('docker cp prod-magma-mme:/tmp/mme_check_run.pcap mme_' + self.testCase_id + '.pcap', '\$', 60)
		else:
			mySSH.command('docker cp prod-oai-mme:/tmp/mme_check_run.pcap mme_' + self.testCase_id + '.pcap', '\$', 60)
		mySSH.command('tshark -r mme_' + self.testCase_id + '.pcap | egrep --colour=never "Tracking area update"', '\$', 60)
		result = re.search('Tracking area update request', mySSH.getBefore())
		if result is not None:
			message = 'UE requested ' + str(mySSH.getBefore().count('Tracking area update request')) + 'Tracking area update request(s)'
		else:
			message = 'No Tracking area update request'
		logging.debug(message)
		mySSH.command('docker cp prod-oai-spgwc:/tmp/spgwc_check_run.pcap spgwc_' + self.testCase_id + '.pcap', '\$', 60)
		mySSH.command('docker cp prod-oai-spgwu-tiny:/tmp/spgwu_check_run.pcap spgwu_' + self.testCase_id + '.pcap', '\$', 60)
		# Remove all
		mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
		if self.isMagmaUsed:
			listOfContainers = 'prod-cassandra prod-oai-hss prod-magma-mme prod-oai-spgwc prod-oai-spgwu-tiny prod-redis'
			nbContainers = 6
		else:
			listOfContainers = 'prod-cassandra prod-oai-hss prod-oai-mme prod-oai-spgwc prod-oai-spgwu-tiny'
			nbContainers = 5
		# Checking for additional services
		mySSH.command('docker-compose config', '\$', 5)
		configResponse = mySSH.getBefore()
		if configResponse.count('trf_gen') == 1:
			listOfContainers += ' prod-trf-gen'
			nbContainers += 1

		mySSH.command('docker-compose down', '\$', 60)
		mySSH.command('docker volume prune --force || true', '\$', 60)
		mySSH.command('docker inspect --format=\'{{.State.Health.Status}}\' ' + listOfContainers, '\$', 10)
		noMoreContainerNb = mySSH.getBefore().count('No such object')
		mySSH.command('docker inspect --format=\'{{.Name}}\' prod-oai-public-net prod-oai-private-net', '\$', 10)
		noMoreNetworkNb = mySSH.getBefore().count('No such object')
		mySSH.close()
		if noMoreContainerNb == nbContainers and noMoreNetworkNb == 2:
			logging.debug('Undeployment OK')
			HTML.CreateHtmlTestRowQueue(self.Type, 'OK', [message])
		else:
			logging.debug('Undeployment went wrong')
			HTML.CreateHtmlTestRowQueue(self.Type, 'KO', [message])

	def LogCollectHSS(self):
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
		mySSH.command('rm -f hss.log.zip', '\$', 5)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker inspect prod-oai-hss', '\$', 10)
			result = re.search('No such object', mySSH.getBefore())
			if result is not None:
				mySSH.command('cd ../logs', '\$', 5)
				mySSH.command('rm -f hss.log.zip', '\$', 5)
				mySSH.command('zip hss.log.zip hss_*.*', '\$', 60)
				mySSH.command('mv hss.log.zip ../scripts', '\$', 60)
			else:
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-hss:/openair-hss/hss_check_run.log .', '\$', 60)
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-hss:/tmp/hss_check_run.pcap .', '\$', 60)
				mySSH.command('zip hss.log.zip hss_check_run.*', '\$', 60)
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			logging.debug('LogCollect is bypassed for that variant')
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			logging.debug('LogCollect is bypassed for that variant')
		elif re.match('OAI', self.Type, re.IGNORECASE) or re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('zip hss.log.zip hss*.log', '\$', 60)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm hss*.log', '\$', 5)
			if re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
				mySSH.command('zip hss.log.zip logs/hss*.* *.pcap', '\$', 60)
				mySSH.command('echo ' + self.Password + ' | sudo -S rm -f logs/hss*.* *.pcap', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cp /opt/hss_sim0609/hss.log .', '\$', 60)
			mySSH.command('zip hss.log.zip hss.log', '\$', 60)
		else:
			logging.error('This option should not occur!')
		mySSH.close()

	def LogCollectMME(self):
		if self.Type != 'OC-OAI-CN5G':
			mySSH = SSH.SSHConnection()
			mySSH.open(self.IPAddress, self.UserName, self.Password)
			mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
			mySSH.command('rm -f mme.log.zip', '\$', 5)
		else:
			mySSH = cls_cmd.getConnection(self.IPAddress)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker inspect prod-oai-mme', '\$', 10)
			result = re.search('No such object', mySSH.getBefore())
			if result is not None:
				mySSH.command('cd ../logs', '\$', 5)
				mySSH.command('rm -f mme.log.zip', '\$', 5)
				mySSH.command('zip mme.log.zip mme_*.*', '\$', 60)
				mySSH.command('mv mme.log.zip ../scripts', '\$', 60)
			else:
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-mme:/openair-mme/mme_check_run.log .', '\$', 60)
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-mme:/tmp/mme_check_run.pcap .', '\$', 60)
				mySSH.command('zip mme.log.zip mme_check_run.*', '\$', 60)
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath + '/logs','\$', 5)
			mySSH.command('cp -f /tmp/oai-cn5g-v1.5.pcap .','\$', 30)
			mySSH.command('zip mme.log.zip oai-amf.log oai-nrf.log oai-cn5g*.pcap','\$', 30)
			mySSH.command('mv mme.log.zip ' + self.SourceCodePath + '/scripts','\$', 30)
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			mySSH.run('cd ' + self.SourceCodePath + '/logs')
			mySSH.run('zip mme.log.zip oai-amf.log oai-nrf.log oai-cn5g*.pcap')
			mySSH.run('mv mme.log.zip ' + self.SourceCodePath + '/scripts')
		elif re.match('OAI', self.Type, re.IGNORECASE) or re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('zip mme.log.zip mme*.log', '\$', 60)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm mme*.log', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cp /opt/ltebox/var/log/*Log.0 .', '\$', 5)
			mySSH.command('zip mme.log.zip mmeLog.0 s1apcLog.0 s1apsLog.0 s11cLog.0 libLog.0 s1apCodecLog.0 amfLog.0 ngapcLog.0 ngapcommonLog.0 ngapsLog.0', '\$', 60)
		else:
			logging.error('This option should not occur!')
		mySSH.close()

	def LogCollectSPGW(self):
		mySSH = SSH.SSHConnection()
		mySSH.open(self.IPAddress, self.UserName, self.Password)
		mySSH.command('cd ' + self.SourceCodePath + '/scripts', '\$', 5)
		mySSH.command('rm -f spgw.log.zip', '\$', 5)
		if re.match('OAI-Rel14-Docker', self.Type, re.IGNORECASE):
			mySSH.command('docker inspect prod-oai-mme', '\$', 10)
			result = re.search('No such object', mySSH.getBefore())
			if result is not None:
				mySSH.command('cd ../logs', '\$', 5)
				mySSH.command('rm -f spgw.log.zip', '\$', 5)
				mySSH.command('zip spgw.log.zip spgw*.*', '\$', 60)
				mySSH.command('mv spgw.log.zip ../scripts', '\$', 60)
			else:
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-spgwc:/openair-spgwc/spgwc_check_run.log .', '\$', 60)
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-spgwu-tiny:/openair-spgwu-tiny/spgwu_check_run.log .', '\$', 60)
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-spgwc:/tmp/spgwc_check_run.pcap .', '\$', 60)
				mySSH.command('docker cp ' + self.containerPrefix + '-oai-spgwu-tiny:/tmp/spgwu_check_run.pcap .', '\$', 60)
				mySSH.command('zip spgw.log.zip spgw*_check_run.*', '\$', 60)
		elif re.match('OAICN5G', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath + '/logs','\$', 5)
			mySSH.command('zip spgw.log.zip oai-smf.log oai-spgwu.log','\$', 30)
			mySSH.command('mv spgw.log.zip ' + self.SourceCodePath + '/scripts','\$', 30)
		elif re.match('OC-OAI-CN5G', self.Type, re.IGNORECASE):
			mySSH.command('cd ' + self.SourceCodePath + '/logs','\$', 5)
			mySSH.command('zip spgw.log.zip oai-smf.log oai-spgwu.log','\$', 30)
			mySSH.command('mv spgw.log.zip ' + self.SourceCodePath + '/scripts','\$', 30)
		elif re.match('OAI', self.Type, re.IGNORECASE) or re.match('OAI-Rel14-CUPS', self.Type, re.IGNORECASE):
			mySSH.command('zip spgw.log.zip spgw*.log', '\$', 60)
			mySSH.command('echo ' + self.Password + ' | sudo -S rm spgw*.log', '\$', 5)
		elif re.match('ltebox', self.Type, re.IGNORECASE):
			mySSH.command('cp /opt/ltebox/var/log/*Log.0 .', '\$', 5)
			mySSH.command('zip spgw.log.zip xGwLog.0 upfLog.0', '\$', 60)
		else:
			logging.error('This option should not occur!')
		mySSH.close()
