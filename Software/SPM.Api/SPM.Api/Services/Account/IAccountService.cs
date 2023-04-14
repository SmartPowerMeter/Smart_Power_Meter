using SPM.Api.Services.Account.Models;

namespace SPM.Api.Services.Account
{
    public interface IAccountService
    {
        Task RegisterUser(RegisterUserRequest request);

        Task<LoginUserResponse> LoginUser(LoginUserRequest request);

        Task<GetUserDataResponse> GetUserData();
    }
}
