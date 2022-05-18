import yfinance as yf
from datetime import date
from datetime import timedelta
from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
import time as t

def push_stock_data(stock_ticker):
 # Download AMZN data from yahoo finance
 data = yf.download(stock_ticker, start=date.today()-timedelta(days=10), end=date.today())
 data_arr = list(data["Close"])

 print(data_arr)

 # Define ENDPOINT, CLIENT_ID, PATH_TO_CERT, PATH_TO_KEY, PATH_TO_ROOT, MESSAGE, TOPIC, and RANGE
 ENDPOINT = "a13s8kmdddvqm1-ats.iot.us-east-1.amazonaws.com"
 CLIENT_ID = "stockly_macbook_air"
 PATH_TO_CERT = "/Users/admin/Downloads/stockly_macbook_air/certificates/a8f7db97a8cbbfd23f9f047121a0c04f770b1b73b07f89fd5d80e72698af8f7b-certificate.pem.crt"
 PATH_TO_KEY = "/Users/admin/Downloads/stockly_macbook_air/certificates/a8f7db97a8cbbfd23f9f047121a0c04f770b1b73b07f89fd5d80e72698af8f7b-private.pem.key"
 PATH_TO_ROOT = "/Users/admin/Downloads/stockly_macbook_air/certificates/root.pem"
 TOPIC = "stockly/" + stock_ticker
 RANGE = 10
 # Spin up resources
 event_loop_group = io.EventLoopGroup(1)
 host_resolver = io.DefaultHostResolver(event_loop_group)
 client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
 mqtt_connection = mqtt_connection_builder.mtls_from_path(
  endpoint=ENDPOINT,
  cert_filepath=PATH_TO_CERT,
  pri_key_filepath=PATH_TO_KEY,
  client_bootstrap=client_bootstrap,
  ca_filepath=PATH_TO_ROOT,
  client_id=CLIENT_ID,
  clean_session=False,
  keep_alive_secs=6
  )
 print("Connecting to {} with client ID ", ENDPOINT, ", ", CLIENT_ID)
 # Make the connect() call
 connect_future = mqtt_connection.connect()
 # Future.result() waits until a result is available
 connect_future.result()
 print("Connected!")
 # Publish message to server desired number of times.
 print("Begin Publish")
 for i in data_arr:
  mqtt_connection.publish(topic=TOPIC, payload="{:.2f}".format(i), qos=mqtt.QoS.AT_LEAST_ONCE)
  print("Published: " + "{:.2f}".format(i) + " to the topic: " + TOPIC)
  t.sleep(0.1)
 print("Publish End")
 disconnect_future = mqtt_connection.disconnect()
 disconnect_future.result()

push_stock_data("AMZN")