Each server (can interchangably be used as primary/secondary) maintains:
  - Zone file:
      - all other DNS servers
      - all domain : address translations
  - Hash table holding all translations
  - Hash table holding requester IP # requests in last X seconds for RRL
  - Trxn # holding the last file update recieved
  - Stuff for DNSSEC

Each server always processes request unless waiting for zone file if:
  - if just restarted
  - if recieves a txn id newer than its number (i.e it missed one)

If primary is down, login to a different 
  - Have base server that has a zone file and is used to send updates out
  - Login to base server, tell it command
     - it updates itself, 