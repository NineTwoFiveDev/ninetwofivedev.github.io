using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.ConstrainedExecution;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Artifact1
{
    public class Contact
    {
        // Backing fields
        private readonly string _id;
        private string _firstName;
        private string _lastName;
        private string _email;
        private string _phone;
        private string _address;

        public Contact(string contactId, string firstName, string lastName, string email, string phone, string address)
        {
            if (contactId == null || contactId.Length > 10)
            {
                throw new ArgumentException("Contact ID must be non-null and no longer than 10 characters.");
            }
            // Initializing properties so validation logic is applied upon creation
            _id = contactId;
            FirstName = firstName;
            LastName = lastName;
            Email = email;
            Phone = phone;
            Address = address;
        }

        // Read-only property (replaces getContactId)
        public string Id
        {
            get { return _id; }
        }

        // Property for First Name
        public string FirstName
        {
            get { return _firstName; }
            set
            {
                if (value == null || value.Length > 10)
                {
                    // Replaced Java's IllegalArgumentException with C#'s ArgumentException
                    throw new ArgumentException("First name must be non-null and no longer than 10 characters.");
                }
                _firstName = value;
            }
        }

        // Property for Last Name
        public string LastName
        {
            get { return _lastName; }
            set
            {
                if (value == null || value.Length > 10)
                {
                    throw new ArgumentException("Last name must be non-null and no longer than 10 characters.");
                }
                _lastName = value;
            }
        }

        // Property for Email (Added for completeness based on your variables)
        public string Email
        {
            get { return _email; }
            set { _email = value; }
        }

        // Property for Phone
        public string Phone
        {
            get { return _phone; }
            set
            {
                // Replaced Java's .matches() with C#'s Regex.IsMatch()
                if (value == null || value.Length != 10 || !Regex.IsMatch(value, @"^\d{10}$"))
                {
                    throw new ArgumentException("Phone number must be non-null, exactly 10 digits, and contain only digits.");
                }
                _phone = value;
            }
        }

        // Property for Address
        public string Address
        {
            get { return _address; }
            set
            {
                if (value == null || value.Length > 30)
                {
                    throw new ArgumentException("Address must be non-null and no longer than 30 characters.");
                }
                _address = value;
            }
        }
    }
}
