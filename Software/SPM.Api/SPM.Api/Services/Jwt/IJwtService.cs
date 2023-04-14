using SPM.Api.Core.Domain.Models;

namespace SPM.Api.Services.Jwt
{
    public interface IJwtService
    {
        string GenerateToken(User user);
    }
}
