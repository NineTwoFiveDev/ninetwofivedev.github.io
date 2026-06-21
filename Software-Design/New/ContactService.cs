using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Artifact1
{
    public class ContactService
    {
        private readonly Dictionary<string, Contact> _contacts = new Dictionary<string, Contact>();

        /*
         * AddContact()
         * RETURNS: void
         * Description:
         * Adds a contact to the map of contacts if the contactID has not already been taken.
         */
        public void AddContact(Contact contact)
        {
            if (_contacts.ContainsKey(contact.Id))
            {
                throw new ArgumentException("Contact ID already exists.");
            }
            _contacts.Add(contact.Id, contact);
        }

        /*
         * DeleteContact()
         * RETURNS: void
         * Description:
         * Deletes a contact from the list of contacts if found.
         */
        public void DeleteContact(string contactId)
        {
            if (!_contacts.ContainsKey(contactId))
            {
                throw new ArgumentException("Contact ID not found.");
            }
            _contacts.Remove(contactId);
        }

        /*
         * UpdateContact()
         * RETURNS: void
         * Description:
         * Updates a contact based on the updated information provided.
         */
        public void UpdateContact(string contactId, string firstName, string lastName, string email, string phone, string address)
        {
            if (!_contacts.ContainsKey(contactId))
            {
                throw new ArgumentException("Contact ID not found.");
            }

            Contact contact = _contacts[contactId];

            if (firstName != null)
            {
                contact.FirstName = firstName;
            }
            if (lastName != null)
            {
                contact.LastName = lastName;
            }
            // Added the email check and update
            if (email != null)
            {
                contact.Email = email;
            }
            if (phone != null)
            {
                contact.Phone = phone;
            }
            if (address != null)
            {
                contact.Address = address;
            }
        }

        /*
         * GetContact()
         * RETURNS: Contact
         * Description:
         * Returns a contact based on contactId
         */
        public Contact GetContact(string contactId)
        {
            return _contacts.GetValueOrDefault(contactId);
        }
    }
}
