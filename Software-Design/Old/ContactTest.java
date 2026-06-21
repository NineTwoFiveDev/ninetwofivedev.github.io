/****************************
 * Name: 	Jacob Gilliam
 * Course: 	CS-320 
 * Date: 	March 23, 2025
 * Description: This is the ContactTest class. It tests all required functionality in the Contact Class.
 ****************************/
package test.contact;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import main.contact.Contact;

class ContactTest {
	//private global Contact to use for testing.
	private Contact contact;
	
	//Before tests occur, setup a contact to test with.
    @BeforeEach
    void setUp() {
        contact = new Contact("1234567890", "Jacob", "Gilliam", "1234567890", "123 Main St");
    }
    
    //Simple test to determine if the constructor is valid. 
    @Test
    void testContactConstructorValid() {
        assertNotNull(contact);
        assertEquals("1234567890", contact.getContactId());
        assertEquals("Jacob", contact.getFirstName());
        assertEquals("Gilliam", contact.getLastName());
        assertEquals("1234567890", contact.getPhone());
        assertEquals("123 Main St", contact.getAddress());
    }
    //If a contact is created with an ID that is too long.
    @Test
    void testContactConstructorInvalidIdTooLong() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("12345678901", "Jacob", "Gilliam", "1234567890", "123 Main St"));
    }
    //if the id is null
    @Test
    void testContactConstructorInvalidIdNull() {
        assertThrows(IllegalArgumentException.class, () -> new Contact(null, "Jacob", "Gilliam", "1234567890", "123 Main St"));
    }
    //first name is too long
    @Test
    void testContactConstructorInvalidFirstNameTooLong() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacobbbbbbbbbbbb", "Gilliam", "1234567890", "123 Main St"));
    }
    //first name is null
    @Test
    void testContactConstructorInvalidFirstNameNull() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", null, "Gilliam", "1234567890", "123 Main St"));
    }
    //last name too long
    @Test
    void testContactConstructorInvalidLastNameTooLong() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliammmmmmmmmmm", "1234567890", "123 Main St"));
    }
    //last name null
    @Test
    void testContactConstructorInvalidLastNameNull() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", null, "1234567890", "123 Main St"));
    }
    //phone number too long
    @Test
    void testContactConstructorInvalidPhoneTooLong() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", "12345678901", "123 Main St"));
    }
    //phone number too short
    @Test
    void testContactConstructorInvalidPhoneTooShort() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", "123456789", "123 Main St"));
    }
    //phone number null
    @Test
    void testContactConstructorInvalidPhoneNull() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", null, "123 Main St"));
    }
    //non-numeric phone number
    @Test
    void testContactConstructorInvalidPhoneNonNumeric() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", "123456789a", "123 Main St"));
    }
    //address is too long
    @Test
    void testContactConstructorInvalidAddressTooLong() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", "1234567890", "123 Main Street Very Very Long Address"));
    }
    //address is null
    @Test
    void testContactConstructorInvalidAddressNull() {
        assertThrows(IllegalArgumentException.class, () -> new Contact("1234567890", "Jacob", "Gilliam", "1234567890", null));
    }
    //set only first name with the same tests
    @Test
    void testSetFirstName() {
        contact.setFirstName("Jay");
        assertEquals("Jay", contact.getFirstName());
        assertThrows(IllegalArgumentException.class, () -> contact.setFirstName("Jayyyyyyyyyyyy"));
        assertThrows(IllegalArgumentException.class, () -> contact.setFirstName(null));
    }
    //set only last name with the same tests
    @Test
    void testSetLastName() {
        contact.setLastName("Smith");
        assertEquals("Smith", contact.getLastName());
        assertThrows(IllegalArgumentException.class, () -> contact.setLastName("Smithhhhhhhhh"));
        assertThrows(IllegalArgumentException.class, () -> contact.setLastName(null));
    }
    //set only phone number with the same tests
    @Test
    void testSetPhone() {
        contact.setPhone("9876543210");
        assertEquals("9876543210", contact.getPhone());
        assertThrows(IllegalArgumentException.class, () -> contact.setPhone("12345"));
        assertThrows(IllegalArgumentException.class, () -> contact.setPhone("12345678901"));
        assertThrows(IllegalArgumentException.class, () -> contact.setPhone("123456789a"));
        assertThrows(IllegalArgumentException.class, () -> contact.setPhone(null));
    }
    //set only address with the same tests
    @Test
    void testSetAddress() {
        contact.setAddress("456 Oak Ave");
        assertEquals("456 Oak Ave", contact.getAddress());
        assertThrows(IllegalArgumentException.class, () -> contact.setAddress("123 Main Street Very Very Long Address"));
        assertThrows(IllegalArgumentException.class, () -> contact.setAddress(null));
    }
}
