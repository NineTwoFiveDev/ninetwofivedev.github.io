/****************************
 * Name: 	Jacob Gilliam
 * Course: 	CS-320 
 * Date: 	March 23, 2025
 * Description: This is the ContactServiceTest class. It tests all required functionality of the ContactService class.
 ****************************/
package test.contact;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import main.contact.Contact;
import main.contact.ContactService;

class ContactServiceTest {
	//global variables to be utilized in testing.
	private ContactService contactService;
    private Contact contact1;
    
    //Initialization before tests
    @BeforeEach
    void setUp() {
        contactService = new ContactService();
        contact1 = new Contact("1234567890", "Jacob", "Gilliam", "1234567890", "123 Main St");
    }
    //test adding a contact
    @Test
    void testAddContact() {
        contactService.addContact(contact1);
        assertEquals(contact1, contactService.getContact("1234567890"));
        assertThrows(IllegalArgumentException.class, () -> contactService.addContact(contact1));
    }
    //deleting a contact
    @Test
    void testDeleteContact() {
        contactService.addContact(contact1);
        contactService.deleteContact("1234567890");
        assertNull(contactService.getContact("1234567890"));
        assertThrows(IllegalArgumentException.class, () -> contactService.deleteContact("1234567890"));
    }
    //updating a contact
    @Test
    void testUpdateContact() {
        contactService.addContact(contact1);
        contactService.updateContact("1234567890", "Jay", "Smith", "9998887777", "789 Pine Ln");
        assertEquals("Jay", contact1.getFirstName());
        assertEquals("Smith", contact1.getLastName());
        assertEquals("9998887777", contact1.getPhone());
        assertEquals("789 Pine Ln", contact1.getAddress());
        assertThrows(IllegalArgumentException.class, () -> contactService.updateContact("9999999999", "test", "test", "1234567890", "test"));
    }
}
