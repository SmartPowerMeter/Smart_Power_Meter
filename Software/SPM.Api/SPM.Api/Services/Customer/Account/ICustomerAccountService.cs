using SPM.Api.Services.Customer.Account.Models;

namespace SPM.Api.Services.Customer.Account
{
    public interface ICustomerAccountService
    {
        Task RegisterUser(RegisterUserRequest request);

        Task<LoginUserResponse> LoginUser(LoginUserRequest request);

        Task<GetUserDataResponse> GetUserData();

        Task ChangePassword(string newPassword);

        Task RecoverPassword(string email);
    }
}
