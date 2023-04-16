import { Component, OnInit } from '@angular/core';
import { Chart } from 'chart.js';
import { ChartConfiguration } from 'chart.js';
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
  lineChart1: any;
  lineChart2: any;
  lineChart3: any;
  lineChart4: any;
  lineChart5: any;
  lineChart6: any;
  lineChartCon1: number[] = [1,2,3,4];
  lineChartLabels1: string[] = ['dges', 'xval', 'zeg', 'mazeg']; 
  lineChartCon2: number[];
  lineChartLabels2: string[]; 
  lineChartCon3: number[];
  lineChartLabels3: string[];
  lineChartCon4: number[];
  lineChartLabels4: string[];
  lineChartCon5: number[];
  lineChartLabels5: string[];
  lineChartCon6: number[];
  lineChartLabels6: string[];

  constructor(public _api: ApiService,) {
    this._api.GetUser().subscribe((res)=>{
      this.user = res;
    });
   }

  ngOnInit(): void {
    this.createLineChart1();
    this.createLineChart2();
  }

  clicked() {
    this.isToggleChecked = !this.isToggleChecked;
  }


  createLineChart1() {
    this.lineChart1 = new Chart("MyLineChart1", {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: this.lineChartLabels1,
        datasets: [
          {
            label: "Consumed",
            data: this.lineChartCon1,
            backgroundColor: "limegreen",
            borderColor: "#7ADC24",
            tension: 0.2,
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
      
    });
  }

  createLineChart2() {
    this.lineChart2 = new Chart("MyLineChart2", {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: this.lineChartLabels1,
        datasets: [
          {
            label: "Consumed",
            data: [5,4,3,2],
            backgroundColor: "limegreen",
            borderColor: "#7ADC24",
            tension: 0.2,
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }



}
