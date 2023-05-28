using SPM.Api.Core.Extensions;
using SPM.Api.Core.Domain.Enums;
using Microsoft.AspNetCore.Authorization;

namespace SPM.Api.Policies
{
    public class AdminUserRequirement : IAuthorizationRequirement
    {
    }

    public class AdminUserHandler : AuthorizationHandler<AdminUserRequirement>
    {
        private readonly IHttpContextAccessor _httpContextAccessor;

        public AdminUserHandler(IHttpContextAccessor httpContextAccessor)
        {
            _httpContextAccessor = httpContextAccessor;
        }

        protected override Task HandleRequirementAsync(AuthorizationHandlerContext context, AdminUserRequirement requirement)
        {
            var httpContext = _httpContextAccessor.HttpContext;

            var user = httpContext?.ToLoggedInUser();

            if (user == null)
            {
                context.Fail();
                return Task.CompletedTask;
            }

            var isAdmin = user.UserType.Equals(UserType.Admin);

            if (!isAdmin)
            {
                context.Fail();
                return Task.CompletedTask;
            }

            context.Succeed(requirement);

            return Task.CompletedTask;
        }
    }
}
