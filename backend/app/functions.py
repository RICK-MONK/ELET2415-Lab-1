 #!/usr/bin/python3


#################################################################################################################################################
#                                                    CLASSES CONTAINING ALL THE APP FUNCTIONS                                                                                                    #
#################################################################################################################################################


class DB:
    def getAllUpdates(self):
        '''RETURNS ALL DOCUMENTS IN THE UPDATE COLLECTION'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s/' % (self.server, self.port), tls=self.tls)
            # Find all documents, exclude the MongoDB _id, and sort by latest
            result = list(remotedb.ELET2415.update.find({}, {'_id': 0}).sort('timestamp', -1))
            return result
        except Exception as e:
            print("getAllUpdates error ", str(e))
            return []
    def __init__(self):
        # REPLACE THE OLD AUTHENTICATION STRING WITH THIS ONE:
        self.client = MongoClient("mongodb://localhost:27017/")
        
        self.db     = self.client.ELET2415
        self.collection = self.db.update

    def remoteMongo(self, uri, tls=False):
        '''
        HELPER FUNCTION TO ESTABLISH MONGODB CONNECTION
        '''
        try:
            # Create a new client and connect to the server
            client = MongoClient(uri, tls=tls, serverSelectionTimeoutMS=5000)
            
            # Trigger a simple command to verify connection
            client.server_info()
            
            return client
        except Exception as e:
            print(f"remoteMongo Connection Error: {e}")
            return None
    def __init__(self,Config):

        from math import floor
        from os import getcwd
        from os.path import join
        from json import loads, dumps, dump
        from datetime import timedelta, datetime, timezone 
        from pymongo import MongoClient , errors, ReturnDocument
        from urllib import parse
        from urllib.request import  urlopen 
        from bson.objectid import ObjectId  
       
      
        self.Config                         = Config
        self.getcwd                         = getcwd
        self.join                           = join 
        self.floor                      	= floor 
        self.loads                      	= loads
        self.dumps                      	= dumps
        self.dump                       	= dump  
        self.datetime                       = datetime
        self.ObjectId                       = ObjectId 
        self.server			                = Config.DB_SERVER
        self.port			                = Config.DB_PORT
        self.username                   	= parse.quote_plus(Config.DB_USERNAME)
        self.password                   	= parse.quote_plus(Config.DB_PASSWORD)
        self.remoteMongo                	= MongoClient
        self.ReturnDocument                 = ReturnDocument
        self.PyMongoError               	= errors.PyMongoError
        self.BulkWriteError             	= errors.BulkWriteError  
        self.tls                            = False # MUST SET TO TRUE IN PRODUCTION


    def __del__(self):
            # Delete class instance to free resources
            pass
 


    ####################
    # LAB 1 FUNCTIONS  #
    ####################
    
    ####################
    # LAB 1 FUNCTIONS  #
    ####################
    
    def addUpdate(self, data):
        '''ADD A NEW STORAGE LOCATION TO COLLECTION'''
        try:
            # REMOVED USERNAME/PASSWORD FROM HERE
            remotedb = self.remoteMongo('mongodb://%s:%s/' % (self.server, self.port), tls=self.tls)
            result = remotedb.ELET2415.update.insert_one(data)
            return True
        except Exception as e:
            msg = str(e)
            if "duplicate" not in msg:
                print("addUpdate error ", msg)
            return False

    def numberFrequency(self):
        '''RETURNS A LIST OF OBJECTS WITH NUMBER AND FREQUENCY'''
        try:
            # REMOVED USERNAME/PASSWORD FROM HERE
            remotedb = self.remoteMongo('mongodb://%s:%s/' % (self.server, self.port), tls=self.tls)
            
            result = list(remotedb.ELET2415.update.aggregate([
                {
                    '$group': {
                        '_id': '$number',
                        'frequency': {'$sum': 1}
                    }
                },
                {
                    '$sort': {'_id': 1}
                },
                {
                    '$project': {
                        '_id': 0,
                        'number': '$_id',
                        'frequency': 1
                    }
                }
            ]))
            return result
        except Exception as e:
            msg = str(e)
            print("numberFrequency error ", msg)
            return []

    def onCount(self, LED_Name):
        '''RETURNS COUNT OF HOW MANY TIMES A SPECIFIC LED WAS ON'''
        try:
            remotedb = self.remoteMongo('mongodb://%s:%s/' % (self.server, self.port), tls=self.tls)
            result = remotedb.ELET2415.update.count_documents({LED_Name: 1})
            return result
        except Exception as e:
            print("onCount error ", str(e))
            return 0


def main():
    from config import Config
    from time import time, ctime, sleep
    from math import floor
    from datetime import datetime, timedelta
    one = DB(Config)
 
 
    start = time() 
    end = time()
    print(f"completed in: {end - start} seconds")
    
if __name__ == '__main__':
    main()


    
