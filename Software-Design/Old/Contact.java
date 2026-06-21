/****************************
 * Name: 	Jacob Gilliam
 * Course: 	CS-320 
 * Date: 	March 23, 2025
 * Description: This is the contact class. It creates and stores contact information.
 ****************************/
package main.contact;

public class Contact {
	//Private class variables
    private final String ID; //final due to never changing.
    private String firstName;
    private String lastName;
    private String phone;
    private String address;
    
    /*
     * Contact Constructor
     * Creates a Contact based on a Unique ContactID, a persons first name, last name, phone number, and address.
     */
    public Contact(String contactId, String firstName, String lastName, String phone, String address) {
        if (contactId == null || contactId.length() > 10) {
            throw new IllegalArgumentException("Contact ID must be non-null and no longer than 10 characters.");
        }
        this.ID = contactId;
        this.setFirstName(firstName);
        this.setLastName(lastName);
        this.setPhone(phone);
        this.setAddress(address);
    }
    /*
     * getContactId()
     * RETURNS: String - ID
     */
    public String getContactId() {
        return this.ID;
    }
    

    /*
     * getFirstName()
     * RETURNS: String - first name
     */
    public String getFirstName() {
        return this.firstName;
    }

    /*
     * setFirstName()
     * RETURNS: void
     * Description:
     * Sets a contact's first name to the provided string if all requirements are met.
     */
    public void setFirstName(String firstName) {
        if (firstName == null || firstName.length() > 10) {
            throw new IllegalArgumentException("First name must be non-null and no longer than 10 characters.");
        }
        this.firstName = firstName;
    }

    /*
     * getLastName()
     * RETURNS: String - last name
     */
    public String getLastName() {
        return this.lastName;
    }

    /*
     * setLastName()
     * RETURNS: void
     * Description:
     * Sets a contact's last name to the provided string if all requirements are met.
     */
    public void setLastName(String lastName) {
        if (lastName == null || lastName.length() > 10) {
            throw new IllegalArgumentException("Last name must be non-null and no longer than 10 characters.");
        }
        this.lastName = lastName;
    }

    /*
     * getPhone()
     * RETURNS: String - phone number
     */
    public String getPhone() {
        return this.phone;
    }

    /*
     * setPhone()
     * RETURNS: void
     * Description:
     * Sets a contact's phone number to the provided string if all requirements are met.
     */
    public void setPhone(String phone) {
        if (phone == null || phone.length() != 10 || !phone.matches("\\d{10}")) {
            throw new IllegalArgumentException("Phone number must be non-null, exactly 10 digits, and contain only digits.");
        }
        this.phone = phone;
    }

    /*
     * getAddress()
     * RETURNS: String - address
     */
    public String getAddress() {
        return this.address;
    }

    /*
     * setAddress()
     * RETURNS: void
     * Description:
     * Sets a contact's address to the provided string if all requirements are met.
     */
    public void setAddress(String address) {
        if (address == null || address.length() > 30) {
            throw new IllegalArgumentException("Address must be non-null and no longer than 30 characters.");
        }
        this.address = address;
    }
}