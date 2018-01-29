# beast 

 - http / https protocol handling 
 - use boost.beast library 
   - not as protocol layer, but as library 
   - integrate with net::service and ssl_protocol 
     
## architecture 

 - ssl_protocol 
   - handles ssl and passes raw bytes message to upper layer 
   - use botan or openssl 
   - decide after looking at boost.beast

 - http_service
   - parse raw bytes and provides http service 
   - use beast here  
   - has task actors to provide service 
   - has a URL to argos to provide monitoring service
