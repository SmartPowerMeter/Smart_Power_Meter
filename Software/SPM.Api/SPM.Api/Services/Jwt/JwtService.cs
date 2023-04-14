using System.Text;
using SPM.Api.Core.Constants;
using System.Security.Claims;
using SPM.Api.Core.Domain.Models;
using Microsoft.IdentityModel.Tokens;
using System.IdentityModel.Tokens.Jwt;

namespace SPM.Api.Services.Jwt
{
    public class JwtService : IJwtService
    {
        private readonly IConfiguration _configuration;

        public JwtService(IConfiguration configuration)
        {
            _configuration = configuration;
        }

        public string GenerateToken(User user)
        {
            var securityKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_configuration["Jwt:Key"]));
            var credentials = new SigningCredentials(securityKey, SecurityAlgorithms.HmacSha256);

            var claims = new[] {
                new Claim(ClaimsConstants.Id, user.Id.ToString()),
                new Claim(ClaimTypes.Email, user.Email)
            };

            var token = new JwtSecurityToken(_configuration["Jwt:Issuer"],
              _configuration["Jwt:Issuer"],
              claims,
              DateTime.Now,
              DateTime.Now.AddMinutes(int.Parse(_configuration["Jwt:ExpirationTimeInMinutes"])),
              credentials);

            return new JwtSecurityTokenHandler().WriteToken(token);
        }
    }
}
