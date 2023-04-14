using SPM.Api.Core.WorkContexts.Models;

namespace SPM.Api.Core.WorkContexts
{
    public class WorkContext : IWorkContext
    {
        public LoggedInUser CurrentUser { get; private set; }

        public WorkContext()
        {
            
        }

        public void SetUser(LoggedInUser loggedInUser)
        {
            CurrentUser = loggedInUser;
        }
    }
}
