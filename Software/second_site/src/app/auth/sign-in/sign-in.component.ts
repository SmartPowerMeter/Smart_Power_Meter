import { Component, OnDestroy, OnInit } from "@angular/core";
import {
  FormBuilder,
  FormControl,
  FormGroup,
  Validators,
} from "@angular/forms";
import { Router } from "@angular/router";
import { Subscription } from "rxjs";
import { UserService } from "src/app/services/user.service";
import { ApiService } from "../../services/api.service";

@Component({
  selector: "app-sign-in",
  templateUrl: "./sign-in.component.html",
  styleUrls: ["./sign-in.component.scss"],
})
export class SignInComponent implements OnInit, OnDestroy {
  public form: FormGroup = new FormGroup({});
  private Email_Value: string = "";
  private forDestr: any;
  private forDestr_1: any;
  public input_valid: string = "";
  public input_valid_pass: string = "";

  public valid_1: boolean = true;
  public valid_2: boolean = true;

  public inputCredintial: string = "";
  // public Password_Value:string="";

  constructor(
    private fb: FormBuilder,
    public _api: ApiService,
    private router: Router,
    private userService: UserService
  ) {
    if (localStorage.getItem("Credentials")) {
      const email = JSON.parse(localStorage.getItem("Credentials") as string);
      this.inputCredintial = email.email;
    } else {
      this.inputCredintial = "";
    }
    this.form = this.fb.group({
      email: ["", Validators.required],
      password: ["", Validators.required],
      check: ["", []],
    });
  }

  ngOnInit() {
    this.forDestr = this.form.get("email")?.valueChanges.subscribe((res) => {
      if (this.form.get("email")?.status == "VALID") {
        this.input_valid = "input-success";
        this.valid_1 = false;
      } else if (this.form.get("email")?.status == "INVALID") {
        this.input_valid = "input-warn";
        this.valid_1 = true;
      }
    });

    this.forDestr_1 = this.form
      .get("password")
      ?.valueChanges.subscribe((res) => {
        if (this.form.get("password")?.status == "VALID") {
          this.input_valid_pass = "input-success";
          this.valid_2 = false;
        } else if (this.form.get("password")?.status == "INVALID") {
          this.input_valid_pass = "input-warn";
          this.valid_2 = true;
        }
      });
  }

  onSubmit() {
    //(this.form.get("check") as FormControl).value  ამით ხდება მოჩეკილის ნახვა
    const Email = (this.form.get("email") as FormControl).value;
    // localStorage.setItem(`Credentials`, JSON.stringify({ email: Email }));
    // }
    this._api
      .LogInPost(this.form.get("email")?.value, this.form.get("password")?.value)
      .subscribe(
        (res) => {
          // if (res[0] !== null || res[0] !== undefined) {
            localStorage.setItem("auth_token", res.accessToken);
            // localStorage.setItem("userId", res[0]["id"]);
            //this.userService.setActiveUser(res.user);

            /* ამოკლედ თუ გინდათ წამატების შემთხვევაში დალოგინება dashboard-ის მაგივრად დაწერეთ როუტი რომელიც გადაიყვანს იუზერს ავტორიზაციის გვერდიდან მომხმარებლის დეფოლტ გვერდამდე/dashboard/what ever-მდე */
            // მაგალითად ასე
            this.router.navigate(["/dashboard"]);
          
        },
        (error) => {
          const passwordError = document.getElementById('password-error');
          passwordError!.style.display = 'block'; // display error message
        return;
        }
      );
  }

  ngOnDestroy(): void {
    (this.forDestr as Subscription).unsubscribe();
    (this.forDestr_1 as Subscription).unsubscribe();
  }
}
