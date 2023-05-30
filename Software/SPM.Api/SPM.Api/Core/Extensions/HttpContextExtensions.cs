using SPM.Api.Core.Constants;
using System.Security.Claims;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Core.WorkContexts.Models;

namespace SPM.Api.Core.Extensions
{
    public static class HttpContextExtensions
    {
        public static LoggedInUser ToLoggedInUser(this HttpContext context)
        {
            if (context.User.Identity is ClaimsIdentity identity && identity.IsAuthenticated)
            {
                return new LoggedInUser
                {
                    Id = int.Parse(identity.FindFirst(ClaimsConstants.Id).Value),
                    Email = identity.FindFirst(ClaimTypes.Email).Value,
                    UserType = (UserType)int.Parse(identity.FindFirst(ClaimsConstants.UserType).Value)
                };
            }

            return new LoggedInUser();
        }
    }
}
