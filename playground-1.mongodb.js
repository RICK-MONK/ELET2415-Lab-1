// Select the database to use.
use('ELET2415');

// Create the collection.
db.createCollection('update');

// Create the user (REQUIRED for Python to connect).
// IMPORTANT: Change "root" and "pass" below to match your functions.py file!
db.createUser({
  user: "root",
  pwd: "pass",
  roles: [{ role: "readWrite", db: "ELET2415" }]
});