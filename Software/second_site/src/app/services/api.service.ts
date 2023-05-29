import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { Router } from "@angular/router";
import { map } from "rxjs/operators";
import { User } from "../auth/user.model";

interface UsersDataType {
  agree_terms?: boolean;
  Email: string;
  full_name?: string;
  Password: string;
  isConnected?: boolean;
  auth_token?: string;
}

@Injectable({
  providedIn: "root",
})
export class ApiService {
  constructor(private _http: HttpClient, private router: Router) {}

  /* რეგისტრაციის  რექვესთებისათვის გამოვიყენეთ მეთოდი POST შემდეგ მომხმარებლის პირად ინფორმაციაში რომ ჩაგვეწერა ტოკენი გამოვიყენეთ მეთოდი PUT ამის შედეგად უსერშო მივიღეთ ობიექტი სადაც გვაქვს ყველა საჭირო ინფორმაცია იუზერისთვის */

  SignUp(userData: UsersDataType) {
    return this._http
      .post<any>("", {
        //http://localhost:3000/users
        email: userData.Email,
        password: userData.Password,
        Full_Name: userData.full_name,
        Agree_Term: userData.agree_terms,
      })
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  LogIn(email: string, password: string) {
    return this._http
      .get<any>(
        `` //http://localhost:3000/users?email=${email}&&password=${password}
      )
      .pipe(
        map((respone) => {
          return respone;
        })
      );
  }
  GetUser() {
    return this._http
      .get<any>("https://api.smartpowermeter.ge/Account/GetUserData")
      .pipe(
        map((res) => {
          return res;
        })
      );
  }

  LogInPost(email: string, password: string) {
    return this._http
      .post<any>("https://api.smartpowermeter.ge/Account/login", {
        email: email,
        password: password,
      })
      .pipe(
        map((res) => {
          return res;
        })
      );
  }

  SignUpPost(userData: UsersDataType) {
    const first_last_name = userData.full_name?.split(" ");
    return this._http
      .post<any>("https://api.smartpowermeter.ge/Account/register", {
        //http://localhost:3000/users
        email: userData.Email,
        password: userData.Password,
        firstName: first_last_name![0],
        lastName: first_last_name![1],
      })
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  MeasurementPost(
    measurementType: number,
    timeRange: number,
    aggregateDuration: number
  ) {
    return this._http
      .post<any>("https://api.smartpowermeter.ge/Measurement/GetMeasurement", {
        measurementType: measurementType,
        timeRange: timeRange,
        aggregateDuration: aggregateDuration,
      })
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  RecentMeasurementPost(
    measurementType: number,
    timeType: number,
    time: number
  ) {
    return this._http
      .post<any>(
        "https://api.smartpowermeter.ge/Measurement/GetRecentMeasurement",
        {
          measurementType: measurementType,
          timeType: timeType,
          time: time,
        }
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  GetRelayStatus() {
    return this._http
      .get<any>("https://api.smartpowermeter.ge/Relay/GetRelayStatus")
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  SetRelayStatus(Activate: boolean) {
    return this._http
      .post<any>(
        `https://api.smartpowermeter.ge/Relay/SetRelayStatus?activate=${Activate}`,
        {}
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  GetEnergyConsumption(timeRange: number) {
    return this._http
      .get(
        `https://api.smartpowermeter.ge/Measurement/GetEnergyConsumption?timeRange=${timeRange}`
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  ChangePassword(newPassword: string) {
    return this._http
      .post<any>(
        `https://api.smartpowermeter.ge/Account/ChangePassword?newPassword=${newPassword}`,
        {}
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  RecoverPassword(email: string) {
    return this._http
      .post<any>(
        `https://api.smartpowermeter.ge/Account/RecoverPassword?email=${email}`,
        {}
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }

  GetAdminUsers() {
    return this._http
      .get(
        `https://api.smartpowermeter.ge/Admin/Account/GetUsers`
      )
      .pipe(
        map((e) => {
          return e;
        })
      );
  }
//https://api.smartpowermeter.ge/Admin/Relay/SetRelayStatus/1?activate=true

SetAdminRelay(customerId: string, activate: boolean) {
  return this._http
    .post<any>(
      `https://api.smartpowermeter.ge/Admin/Relay/SetRelayStatus/${customerId}?activate=${activate}`,
      {}
    )
    .pipe(
      map((e) => {
        return e;
      })
    );
}

  // აქ გაუგებრია პოსტი რატო მაქ
  // LogIn(email: string, password: string) {
  //   return this._http
  //     .post<any>("http://localhost:3000/login", {
  //       email: email,
  //       password: password,
  //     })
  //     .pipe(
  //       map((respone) => {
  //         return respone;
  //       })
  //     );
  // }

  Get_User_Id_With_Email() {
    return this._http.get<any>("").pipe(
      //http://localhost:3000/users
      map((response) => {
        return response;
      })
    );
  }
  update_user_to_server_with_id(user: User, id: number) {
    return this._http
      .put<any>(``, { ...user }) //http://localhost:3000/users/${id}
      .pipe(
        map((response) => {
          return response;
        })
      );
    /*  თუ დაგჭირდებათ იუზერის სხვა მონაცემების გადაწოდება ამ მეთდისთვის მაშინ   UsersDataType- ინტერფეისში  დაამატეთ თქვენი პარამეტრი ოფშენალად. UsersDataType არის ამავე სერვისში აღწერილი ზემოთ, იუზერის მონაცემები შეგიძლიათ მოიპოვოთ user.service.ts ში get-ერის საშუალებით საიდანაც ამოიღებთ წარმატებით დალოგინებული მომხმარებლის ინფორმაციას. :) წარმატებები*/
  }

  getUserData(id: number) {
    return this._http.get<any>(``); //http://localhost:3000/users/${id}
  }

  DeleteUser(id: number) {
    return this._http.delete<any>(``); //http://localhost:3000/users/${id}
  }

  GetUserWithEmail(Email: string) {
    return this._http
      .get<any>(``) //http://localhost:3000/users?email=${Email}
      .pipe(
        map((response) => {
          return response;
        })
        //http://localhost:3000/users?email=${email}
      );
  }

  GetUserStats(id: number) {
    return this._http.get<any>(``).pipe(
      //http://localhost:3000/data/${id}
      map((response) => {
        return response;
      })
    );
  }
}
