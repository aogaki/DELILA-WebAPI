# Setting up MongoDB and Mongo Express in Docker

**docker-compose up -d**  
Avobe command will start MongoDB and Mongo Express in Docker. The default setup uses authentication. The default username and password are _**root**_ and _**password**_. You can change these in the docker-compose.yml file. First, you need to create a database, collection and user by using mongo shell or Express.

### Using mongo shell:

First, check the container id of MongoDB (mongo, NOT mongo-express) by using the following command:

```bash
docker ps
```

Then, connect to the container and do the first setup by using the following command:

```bash
$ docker ps
$ docker exec -it <container id> /bin/bash
root@<container id>:/# mongosh -u root -p password
test> use DELILA
DELILA> db.createCollection("run_information")
DELILA> db.createUser(
... {
... user:"delila",
... pwd:"eli-np",
... roles:[
... {role:"readWrite", db:"DELILA"}
... ]
... })
{ ok: 1 }

```

Exit from docker container and mongo shell by typing **exit**. All data will be stored in the **mongodb_data** folder.
