import { Component, OnInit } from '@angular/core';
import { ApiService } from 'src/app/services/api.service';

export interface User {
  customerId: number;
  firstName: string;
  lastName: string;
  email: string;
}

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})


export class DashboardComponent implements OnInit {
  isToggleChecked = true;
  user: User;

  constructor(public _api: ApiService,) {
    this._api.GetUser().subscribe((res)=>{
      this.user = res;
    });
   }

  ngOnInit(): void {
  }

  clicked() {
    this.isToggleChecked = !this.isToggleChecked;
  }
}
