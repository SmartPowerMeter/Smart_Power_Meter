using SPM.Api.Core.WorkContexts.Models;

namespace SPM.Api.Core.WorkContexts
{
    public interface IWorkContext
    {
        public LoggedInUser CurrentUser { get; }
    }
}
