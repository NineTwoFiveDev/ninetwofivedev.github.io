from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self, user, password, host='nv-desktop-services.apporto.com', port=32541, db='AAC', col='animals'):
        # Initializing the MongoClient. This helps to 
        # access the MongoDB databases and collections.
        # This is hard-wired to use the aac database, the 
        # animals collection, and the aac user.
        # Definitions of the connection string variables are
        # unique to the individual Apporto environment.
        #
        # You must edit the connection variables below to reflect
        # your own instance of MongoDB!
        #
        # Connection Variables
        #
        USER = user
        PASS = password
        HOST = host
        PORT = port
        DB = db
        COL = col
        #
        # Initialize Connection
        #
        self.client = MongoClient('mongodb://%s:%s@%s:%d' % (USER,PASS,HOST,PORT))
        self.database = self.client['%s' % (DB)]
        self.collection = self.database['%s' % (COL)]

# Complete this create method to implement the C in CRUD.

    """
        Inserts a document into the DB animals collection.

        Args:
            data: The document to insert. Required to be a dict.

        Returns:
            bool: True if the insert is successful, False otherwise.
    """
    def create(self, data):
        if not self.collection:
            raise Exception("Database connection not established. Cannot create the document.")
        
        if data is not None:
            if not isinstance(data, dict):
                raise Exception("Invalid data type. data must be a dictionary.")
            
            try:
                # Insert the document into the collection
                result = self.collection.insert_one(data)
                return result.acknowledged # Returns True if acknowledged, False otherwise
        
            except Exception as e:
                raise Exception(f"An unexpected error occurred during create operation: {e}")
        else:
            raise Exception("Nothing to save, because data parameter is empty")

# Create method to implement the R in CRUD.
    """
    Queries for documents from the DB.

    Args:
        query (dict): A dictionary representing the key/value lookup pair.

    Returns:
        list: A list of dictionaries with the matching documents if successful, an empty list otherwise.
    """
    def read(self, query):
        if not self.collection:
            raise Exception("Database connection not established. Cannot perform read operation.")
        
        if not isinstance(query, dict):
            raise Exception("Invalid query type. query must be a dictionary.")
        
        try:
            # Use find() to query for documents.
            cursor = self.collection.find(query)
            # Convert the cursor to a list of dictionaries
            results = list(cursor)
            return results
        except Exception as e:
            raise Exception(f"An unexpected error occurred during read operation: {e}")
            
# Update method to implement the U in CRUD.
    """
    Updates documents from the DB.

    Args:
        query (dict): A dictionary representing the key/value lookup pair to find documents.
        update_data (dict): A dictionary of key/value pairs to update in the found documents.

    Returns:
        int: The number of documents modified in the collection.
    """

    def update(self, query, update_data):
        if not self.collection:
            raise Exception("Database connection not established. Cannot update the document.")

        if not isinstance(query, dict) or not isinstance(update_data, dict):
            raise Exception("Invalid data types. Both query and update_data must be dictionaries.")

        try:
            # Use update_many() to update one or more documents.
            # Using $set to specify the fields to update.
            result = self.collection.update_many(query, {'$set': update_data})
            return result.modified_count
        except Exception as e:
            raise Exception(f"An unexpected error occurred during update operation: {e}")
            
# Delete method to implement the D in CRUD.
    """
    Deletes documents from the DB.

    Args:
        query (dict): A dictionary representing the key/value lookup pair to find documents for deletion.

    Returns:
        int: The number of documents removed from the collection.
    """

    def delete(self, query):
        if not self.collection:
            raise Exception("Database connection not established. Cannot delete the document.")

        if not isinstance(query, dict):
            raise Exception("Invalid query type. query must be a dictionary.")

        try:
            # Use delete_many() to delete one or more documents.
            result = self.collection.delete_many(query)
            return result.deleted_count
        except Exception as e:
            raise Exception(f"An unexpected error occurred during delete operation: {e}")
            
            
            
