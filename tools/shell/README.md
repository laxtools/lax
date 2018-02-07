# Shell 

 features: 

  - query based
  - session kept 

shell uses argos_service to monitor. 

argos_service uses service_directory to forward commands and relay reponse to a shell. 

command : msg_service_base

- context { user or shell, ... }
- req : string (json)
- res : string (json)



## Usage

### CLI: 

 $ shell.exe 

 shell> open ip:port 

 shell> login id password
 shell> logout

 shell> query json 
 shell> query file
 shell> run file 
 shell> spool file 
 shell> spool off 


### Runs commands from file. 

 $ shell.exe file 

  
