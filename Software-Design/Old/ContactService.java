/****************************
 * Name: 	Jacob Gilliam
 * Course: 	CS-320 
 * Date: 	March 23, 2025
 * Description: This is the ContactService class. This handles storing, updating, and deleting contacts.
 ****************************/
package main.contact;

import java.util.HashMap;
import java.util.Map;

public class ContactService {
	
	//Map of all Contacts.
    private final Map<String, Contact> contacts = new HashMap<>();


    /*
     * addContact()
     * RETURNS: void
     * Description:
     * Adds a contact to the map of contacts if the contactID has not already been taken.
     */
    public void addContact(Contact contact) {
        if (contacts.containsKey(contact.getContactId())) {
            throw new IllegalArgumentException("Contact ID already exists.");
        }
        contacts.put(contact.getContactId(), contact);
    }

    /*
     * deleteContact()
     * RETURNS: void
     * Description:
     * Deletes a contact from the list of contacts if found.
     */
    public void deleteContact(String contactId) {
        if (!contacts.containsKey(contactId)) {
            throw new IllegalArgumentException("Contact ID not found.");
        }
        contacts.remove(contactId);
    }

    /*
     * updateContact()
     * RETURNS: void
     * Description:
     * Updates a contact based on the updated information provided.
     */
    public void updateContact(String contactId, String firstName, String lastName, String phone, String address) {
        if (!contacts.containsKey(contactId)) {
            throw new IllegalArgumentException("Contact ID not found.");
        }

        Contact contact = contacts.get(contactId);

        if (firstName != null) {
            contact.setFirstName(firstName);
        }
        if (lastName != null) {
            contact.setLastName(lastName);
        }
        if (phone != null) {
            contact.setPhone(phone);
        }
        if (address != null) {
            contact.setAddress(address);
        }
    }

    /*
     * getContact()
     * RETURNS: Contact
     * Description:
     * Returns a contact based on contactId
     */
    public Contact getContact(String contactId){
        return contacts.get(contactId);
    }
}