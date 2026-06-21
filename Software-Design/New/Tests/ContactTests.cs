using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Artifact1.Tests
{
    [TestClass]
    public class ContactTests
    {
        // private global Contact to use for testing.
        private Contact _contact;

        // Before tests occur, setup a contact to test with.
        [TestInitialize]
        public void SetUp()
        {
            // Added placeholder email to match the 6-parameter C# constructor
            _contact = new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "1234567890", "123 Main St");
        }

        // Simple test to determine if the constructor is valid. 
        [TestMethod]
        public void TestContactConstructorValid()
        {
            Assert.IsNotNull(_contact);
            Assert.AreEqual("1234567890", _contact.Id);
            Assert.AreEqual("Jacob", _contact.FirstName);
            Assert.AreEqual("Gilliam", _contact.LastName);
            Assert.AreEqual("1234567890", _contact.Phone);
            Assert.AreEqual("123 Main St", _contact.Address);
        }

        // If a contact is created with an ID that is too long.
        [TestMethod]
        public void TestContactConstructorInvalidIdTooLong()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("12345678901", "Jacob", "Gilliam", "test@email.com", "1234567890", "123 Main St"));
        }

        // if the id is null
        [TestMethod]
        public void TestContactConstructorInvalidIdNull()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact(null, "Jacob", "Gilliam", "test@email.com", "1234567890", "123 Main St"));
        }

        // first name is too long
        [TestMethod]
        public void TestContactConstructorInvalidFirstNameTooLong()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacobbbbbbbbbbbb", "Gilliam", "test@email.com", "1234567890", "123 Main St"));
        }

        // first name is null
        [TestMethod]
        public void TestContactConstructorInvalidFirstNameNull()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", null, "Gilliam", "test@email.com", "1234567890", "123 Main St"));
        }

        // last name too long
        [TestMethod]
        public void TestContactConstructorInvalidLastNameTooLong()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliammmmmmmmmmm", "test@email.com", "1234567890", "123 Main St"));
        }

        // last name null
        [TestMethod]
        public void TestContactConstructorInvalidLastNameNull()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", null, "test@email.com", "1234567890", "123 Main St"));
        }

        // phone number too long
        [TestMethod]
        public void TestContactConstructorInvalidPhoneTooLong()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "12345678901", "123 Main St"));
        }

        // phone number too short
        [TestMethod]
        public void TestContactConstructorInvalidPhoneTooShort()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "123456789", "123 Main St"));
        }

        // phone number null
        [TestMethod]
        public void TestContactConstructorInvalidPhoneNull()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", null, "123 Main St"));
        }

        // non-numeric phone number
        [TestMethod]
        public void TestContactConstructorInvalidPhoneNonNumeric()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "123456789a", "123 Main St"));
        }

        // address is too long
        [TestMethod]
        public void TestContactConstructorInvalidAddressTooLong()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "1234567890", "123 Main Street Very Very Long Address"));
        }

        // address is null
        [TestMethod]
        public void TestContactConstructorInvalidAddressNull()
        {
            Assert.Throws<ArgumentException>(() =>
                new Contact("1234567890", "Jacob", "Gilliam", "test@email.com", "1234567890", null));
        }

        // set only first name with the same tests
        [TestMethod]
        public void TestSetFirstName()
        {
            _contact.FirstName = "Jay";
            Assert.AreEqual("Jay", _contact.FirstName);
            Assert.Throws<ArgumentException>(() => _contact.FirstName = "Jayyyyyyyyyyyy");
            Assert.Throws<ArgumentException>(() => _contact.FirstName = null);
        }

        // set only last name with the same tests
        [TestMethod]
        public void TestSetLastName()
        {
            _contact.LastName = "Smith";
            Assert.AreEqual("Smith", _contact.LastName);
            Assert.Throws<ArgumentException>(() => _contact.LastName = "Smithhhhhhhhh");
            Assert.Throws<ArgumentException>(() => _contact.LastName = null);
        }

        // set only phone number with the same tests
        [TestMethod]
        public void TestSetPhone()
        {
            _contact.Phone = "9876543210";
            Assert.AreEqual("9876543210", _contact.Phone);
            Assert.Throws<ArgumentException>(() => _contact.Phone = "12345");
            Assert.Throws<ArgumentException>(() => _contact.Phone = "12345678901");
            Assert.Throws<ArgumentException>(() => _contact.Phone = "123456789a");
            Assert.Throws<ArgumentException>(() => _contact.Phone = null);
        }

        // set only address with the same tests
        [TestMethod]
        public void TestSetAddress()
        {
            _contact.Address = "456 Oak Ave";
            Assert.AreEqual("456 Oak Ave", _contact.Address);
            Assert.Throws<ArgumentException>(() => _contact.Address = "123 Main Street Very Very Long Address");
            Assert.Throws<ArgumentException>(() => _contact.Address = null);
        }
    }
}
