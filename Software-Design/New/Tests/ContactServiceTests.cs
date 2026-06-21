using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Artifact1.Tests
{
    [TestClass]
    public class ContactServiceTest
    {
        // global variables to be utilized in testing.
        private ContactService _contactService;
        private Contact _contact1;

        // Initialization before tests
        [TestInitialize]
        public void SetUp()
        {
            _contactService = new ContactService();

            // Added placeholder email to match the 6-parameter C# constructor
            _contact1 = new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "1234567890", "123 Main St");
        }

        // test adding a contact
        [TestMethod]
        public void TestAddContact()
        {
            _contactService.AddContact(_contact1);
            Assert.AreEqual(_contact1, _contactService.GetContact("1234567890"));
            Assert.Throws<ArgumentException>(() => _contactService.AddContact(_contact1));
        }

        // deleting a contact
        [TestMethod]
        public void TestDeleteContact()
        {
            _contactService.AddContact(_contact1);
            _contactService.DeleteContact("1234567890");

            // Replaced assertNull with Assert.IsNull
            Assert.IsNull(_contactService.GetContact("1234567890"));
            Assert.Throws<ArgumentException>(() => _contactService.DeleteContact("1234567890"));
        }

        // updating a contact
        [TestMethod]
        public void TestUpdateContact()
        {
            _contactService.AddContact(_contact1);

            // Added a dummy email ("new@email.com") to match the updated 6-parameter UpdateContact method
            _contactService.UpdateContact("1234567890", "Jay", "Smith", "new@email.com", "9998887777", "789 Pine Ln");

            // Using C# property access instead of getter methods
            Assert.AreEqual("Jay", _contact1.FirstName);
            Assert.AreEqual("Smith", _contact1.LastName);
            Assert.AreEqual("new@email.com", _contact1.Email); // Added an assertion for the new email field
            Assert.AreEqual("9998887777", _contact1.Phone);
            Assert.AreEqual("789 Pine Ln", _contact1.Address);

            // Added 'null' for the email parameter in the exception test to match the method signature
            Assert.Throws<ArgumentException>(() =>
                _contactService.UpdateContact("9999999999", "test", "test", null, "1234567890", "test"));
        }
    }
}
