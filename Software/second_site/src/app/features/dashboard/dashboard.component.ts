import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {
  isToggleChecked = true;

  constructor() { }

  ngOnInit(): void {
  }

  clicked() {
    this.isToggleChecked = !this.isToggleChecked;
  }
}
