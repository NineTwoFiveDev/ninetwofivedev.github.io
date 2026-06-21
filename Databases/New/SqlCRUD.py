import sqlite3


class SecureAnimalShelter:
    def __init__(self, db_path='aac_rescue.db'):
        self.db_path = db_path
        self._initialize_db()

    def _initialize_db(self):
        """Creates the normalized SQL tables if they do not exist."""
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()

            # Table 1: Core Animal Data
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS Animals (
                    animal_id TEXT PRIMARY KEY,
                    animal_type TEXT,
                    breed TEXT,
                    name TEXT,
                    sex_upon_outcome TEXT,
                    age_upon_outcome_in_weeks REAL,
                    location_lat REAL,
                    location_long REAL
                )
            ''')

            # Table 2: Outcomes (Foreign Key linked to Animals)
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS Outcomes (
                    outcome_id INTEGER PRIMARY KEY AUTOINCREMENT,
                    animal_id TEXT,
                    outcome_type TEXT,
                    FOREIGN KEY (animal_id) REFERENCES Animals(animal_id)
                )
            ''')
            conn.commit()

    def check_permissions(self, user_role, required_roles):
        """RBAC Security check."""
        if user_role not in required_roles:
            raise PermissionError(f"Security Alert: User role '{user_role}' is not authorized to perform this action.")

    def read_rescues(self, user_role, rescue_type='reset'):
        """Queries the SQL database based on the dashboard filter."""
        # Security Enhancement: Verify user is allowed to read
        self.check_permissions(user_role, ['viewer', 'editor', 'admin'])

        # SQL JOIN to combine our normalized tables
        query = """
            SELECT a.animal_id, a.animal_type, a.breed, a.name, a.sex_upon_outcome, 
                   a.age_upon_outcome_in_weeks, a.location_lat, a.location_long, o.outcome_type 
            FROM Animals a
            LEFT JOIN Outcomes o ON a.animal_id = o.animal_id
            WHERE 1=1
        """

        # Translate the old MongoDB JSON queries into SQL WHERE clauses
        if rescue_type == 'water':
            query += " AND a.animal_type = 'Dog' AND a.breed IN ('Labrador Retriever Mix', 'Chesapeake Bay Retriever', 'Newfoundland') AND a.sex_upon_outcome = 'Intact Female' AND a.age_upon_outcome_in_weeks BETWEEN 26 AND 156"
        elif rescue_type == 'mountain':
            query += " AND a.animal_type = 'Dog' AND a.breed IN ('German Shepherd', 'Alaskan Malamute', 'Old English Sheepdog', 'Siberian Husky', 'Rottweiler') AND a.sex_upon_outcome = 'Intact Male' AND a.age_upon_outcome_in_weeks BETWEEN 26 AND 156"
        elif rescue_type == 'disaster':
            query += " AND a.animal_type = 'Dog' AND a.breed IN ('Doberman Pinscher', 'German Shepherd', 'Golden Retriever', 'Bloodhound', 'Rottweiler') AND a.sex_upon_outcome = 'Intact Male' AND a.age_upon_outcome_in_weeks BETWEEN 20 AND 300"

        with sqlite3.connect(self.db_path) as conn:
            conn.row_factory = sqlite3.Row  # Returns dict-like rows for the Dash DataTable
            cursor = conn.cursor()
            cursor.execute(query)
            return [dict(row) for row in cursor.fetchall()]

    def delete_animal(self, user_role, animal_id):
        """Example of a destructive action protected by RBAC."""
        self.check_permissions(user_role, ['admin'])  # ONLY admins can delete
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute("DELETE FROM Outcomes WHERE animal_id = ?", (animal_id,))
            cursor.execute("DELETE FROM Animals WHERE animal_id = ?", (animal_id,))
            conn.commit()
            return cursor.rowcount