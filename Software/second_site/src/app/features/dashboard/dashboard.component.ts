import { Component, OnInit } from '@angular/core';
import { ApiService } from 'src/app/services/api.service';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {
  isToggleChecked = true;

  constructor(_api: ApiService) { }

  ngOnInit(): void {
  }

  clicked() {
    this.isToggleChecked = !this.isToggleChecked;
  }
}
