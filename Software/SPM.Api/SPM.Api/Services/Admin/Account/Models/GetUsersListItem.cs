namespace SPM.Api.Services.Admin.Account.Models
{
    public class GetUsersListItem
    {
        public string CustomerId { get; set; }

        public string FirstName { get; set; }

        public string LastName { get; set; }

        public string Email { get; set; }

        public bool CustomerRelayEnabled { get; set; }

        public bool AdminRelayEnabled { get; set; }
    }
}
