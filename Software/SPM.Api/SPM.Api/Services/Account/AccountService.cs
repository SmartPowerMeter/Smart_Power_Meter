using SPM.Api.Data;
using SPM.Api.Core.Helpers;
using SPM.Api.Services.Jwt;
using SPM.Api.Core.Exceptions;
using SPM.Api.Core.WorkContexts;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Core.Domain.Models;
using Microsoft.EntityFrameworkCore;
using System.Security.Authentication;
using SPM.Api.Services.Account.Models;

namespace SPM.Api.Services.Account
{
    public class AccountService : IAccountService
    {
        private readonly IJwtService _jwtService;
        private readonly SPMDbContext _dbContext;
        private readonly IWorkContext _workContext;

        public AccountService(IJwtService jwtService, SPMDbContext dbContext, IWorkContext workContext)
        {
            _jwtService = jwtService;
            _dbContext = dbContext;
            _workContext = workContext;
        }

        public async Task RegisterUser(RegisterUserRequest request)
        {
            var user = await _dbContext.User.FirstOrDefaultAsync(x => x.Email == request.Email);

            if (user != null)
                throw new ArgumentException();

            var hashedPassword = EncryptionHelper.HashPasword(request.Password);

            var userType = request.IsConnected ? UserType.Connected : UserType.Seperate;

            var newUser = new User(request.FirstName, request.LastName, request.Email, hashedPassword, userType);

            await _dbContext.User.AddAsync(newUser);
            await _dbContext.SaveChangesAsync();
        }

        public async Task<LoginUserResponse> LoginUser(LoginUserRequest request)
        {
            var user = await _dbContext.User.FirstOrDefaultAsync(x => x.Email == request.Email);

            if (user == null)
                throw new NotFoundException();

            if (user.Password != EncryptionHelper.HashPasword(request.Password))
                throw new AuthenticationException();

            var response = new LoginUserResponse
            {
                AccessToken = _jwtService.GenerateToken(user),
            };

            return response;
        }

        public async Task<GetUserDataResponse> GetUserData()
        {
            var user = await _dbContext.User.FirstOrDefaultAsync(x => x.Email == _workContext.CurrentUser.Email);

            var response = new GetUserDataResponse
            {
                CustomerId = user.CustomerId,
                Email = user.Email,
                FirstName = user.FirstName,
                LastName = user.LastName
            };

            return response;
        }
    }
}
