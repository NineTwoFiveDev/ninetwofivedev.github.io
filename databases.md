# Artifact 3: Databases

## Project Links
* [Original Code Source](LINK_TO_BEFORE_CODE)
* [Enhanced Code Source](LINK_TO_AFTER_CODE)

---

## Narrative Reflection

For my database enhancement, I chose a client/server dashboard I originally built back in CS-340 (Advanced Programming Concepts). It is a Python and Dash web application that interacts with a backend API. When I first wrote it, I set it up to use a local MongoDB database to store, filter, and retrieve flat JSON records for an animal shelter. I included this piece in my ePortfolio because migrating from a NoSQL setup to a relational database is a very common, real-world engineering scenario. While MongoDB was fine for a simple local project, its flat data structure wasn't sufficiently scalable or secure for an enterprise-level app.

To showcase my database management skills, I completely ripped out the MongoDB backend and migrated the architecture to a relational SQL database using SQLite. I took all that unstructured JSON data and normalized it to Third Normal Form (3NF). By splitting the records into interconnected tables, I was able to eliminate duplicate data and enforce strict data integrity. On top of that, I wrote a custom middleware layer in the Python API to handle Role-Based Access Control (RBAC), locking down the database endpoints so only authorized users can execute commands.

This project was a great way to meet two of the primary course outcomes. It directly hits Outcome 4 because I applied core database engineering practices to design a structured, scalable SQL environment from scratch. It also covers Outcome 5, which focuses on developing a security mindset. By implementing RBAC and using parameterized SQL queries, I ensured the API is protected against privilege escalation and SQL injection attacks. For example, destructive commands like dropping a record are now strictly hard-locked to verified admin roles at the server level, rather than just relying on the frontend UI to hide the delete button.

Honestly, the hardest part of this enhancement was just shifting my brain from a document-based mindset back to a strict relational one. MongoDB lets you get away with throwing everything into a single nested file, so breaking that flat data into interconnected tables without disrupting data relationships during the migration required careful architectural planning. Building the RBAC middleware was also a really valuable learning curve. I had to write logic to intercept the API call, check the user's permissions, and either approve or block the transaction before it even reaches the SQL database. It really hammered home why you have to build security directly into the data layer itself, not just the user interface.


---
[Back to Homepage](index.md)
