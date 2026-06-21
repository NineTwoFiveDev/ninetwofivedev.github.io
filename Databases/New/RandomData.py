import sqlite3
import random
from SqlCRUD import SecureAnimalShelter


def generate_mock_data():
    print("Initializing new SQLite database...")
    # This creates the database and the normalized 3NF tables
    db = SecureAnimalShelter('aac_rescue.db')

    print("Generating Austin Animal Center mock data...")

    # Austin, TX general coordinates to center the map
    base_lat = 30.75
    base_long = -97.48

    # We need specific breeds, ages, and genders to trigger your dashboard filters
    mock_animals = [
        # --- WATER RESCUE MATCHES ---
        # Needs: Dog, (Lab, Chesapeake, Newfoundland), Intact Female, 26-156 weeks
        ("A10001", "Dog", "Labrador Retriever Mix", "Daisy", "Intact Female", 104, "Adoption"),
        ("A10002", "Dog", "Chesapeake Bay Retriever", "Bella", "Intact Female", 52, "Adoption"),
        ("A10003", "Dog", "Newfoundland", "Luna", "Intact Female", 150, "Transfer"),

        # --- MOUNTAIN RESCUE MATCHES ---
        # Needs: Dog, (German Shepherd, Alaskan Malamute, Old English Sheepdog, Siberian Husky, Rottweiler), Intact Male, 26-156 weeks
        ("A20001", "Dog", "German Shepherd", "Rex", "Intact Male", 80, "Adoption"),
        ("A20002", "Dog", "Alaskan Malamute", "Ghost", "Intact Male", 120, "Return to Owner"),
        ("A20003", "Dog", "Siberian Husky", "Balto", "Intact Male", 60, "Adoption"),

        # --- DISASTER RESCUE MATCHES ---
        # Needs: Dog, (Doberman Pinscher, German Shepherd, Golden Retriever, Bloodhound, Rottweiler), Intact Male, 20-300 weeks
        ("A30001", "Dog", "Doberman Pinscher", "Zeus", "Intact Male", 200, "Adoption"),
        ("A30002", "Dog", "Golden Retriever", "Buddy", "Intact Male", 180, "Transfer"),
        ("A30003", "Dog", "Bloodhound", "Duke", "Intact Male", 250, "Adoption"),

        # --- RANDOM/NON-FILTER MATCHES ---
        # To show that the dashboard successfully filters out standard animals
        ("A40001", "Cat", "Domestic Shorthair", "Oliver", "Neutered Male", 40, "Adoption"),
        ("A40002", "Cat", "Siamese", "Leo", "Intact Male", 12, "Transfer"),
        ("A40003", "Dog", "Chihuahua Mix", "Peanut", "Spayed Female", 300, "Adoption"),
        ("A40004", "Dog", "Pit Bull Mix", "Buster", "Neutered Male", 150, "Return to Owner")
    ]

    with sqlite3.connect('aac_rescue.db') as conn:
        cursor = conn.cursor()

        for idx, animal in enumerate(mock_animals):
            a_id, a_type, breed, name, sex, age, outcome = animal

            # Randomize the map markers slightly around Austin so they don't stack on top of each other
            lat = base_lat + random.uniform(-0.1, 0.1)
            lon = base_long + random.uniform(-0.1, 0.1)

            # 1. Insert into Animals Table
            cursor.execute('''
                INSERT OR IGNORE INTO Animals 
                (animal_id, animal_type, breed, name, sex_upon_outcome, age_upon_outcome_in_weeks, location_lat, location_long)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            ''', (a_id, a_type, breed, name, sex, age, lat, lon))

            # 2. Insert into Outcomes Table
            cursor.execute('''
                INSERT INTO Outcomes (animal_id, outcome_type)
                VALUES (?, ?)
            ''', (a_id, outcome))

        conn.commit()

    print("Success! 'aac_rescue.db' has been created and populated.")


if __name__ == "__main__":
    generate_mock_data()