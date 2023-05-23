using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Core.WorkContexts.Models
{
    public class LoggedInUser
    {
        public int Id { get; set; }

        public string Email { get; set; }

        public UserType UserType { get; set; }
    }
}
