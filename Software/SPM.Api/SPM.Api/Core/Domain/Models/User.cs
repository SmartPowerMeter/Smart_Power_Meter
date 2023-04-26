using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Core.Domain.Models
{
    public class User
    {
        #region Properties

        public int Id { get; set; }

        public string CustomerId { get; private set; }

        public string FirstName { get; private set; }

        public string LastName { get; private set; }

        public string Email { get; private set; }

        public string Password { get; private set; }

        public UserType UserType { get; private set; }

        public string BucketAccessToken { get; private set; }

        public bool RelayEnabled { get; private set; }

        #endregion

        public User(string firstName, string lastName, string email, string password, UserType userType)
        {
            FirstName = firstName;
            LastName = lastName;
            Email = email;
            Password = password;
            UserType = userType;
            CustomerId = CreateCustomerId();
        }

        private string CreateCustomerId()
        {
            var guid = Guid.NewGuid().ToString();
            var guidParts = guid.Split('-');

            return guidParts[0] + guidParts[1];
        }

        public string GetBucketName() => $"{CustomerId}_bucket";

        public void SetBucketAccessToken(string token)
        {
            BucketAccessToken = token;
        }

        public void SetRelayState(bool enabled)
        {
            RelayEnabled = enabled;
        }
    }
}
