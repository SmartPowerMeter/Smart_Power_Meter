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
  isToggleChecked: boolean = true;
  modalText: string = 'Power will turn off.';
  user: User;
  selectedOption: string = "Energy";
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
  TimeType: any = {
    Voltage: 1,
    Current: 2,
    Power: 3,
    Frequency: 4,
    "Power Factor": 5,
    Energy: 6,
  };



  constructor(public _api: ApiService,) {
    this._api.GetUser().subscribe((res)=>{
      this.user = res;
    });
  }

  ngOnInit(): void {
    this.createLineCharts();
  }

  clicked() {
    if (!this.isToggleChecked) 
      this.modalText = 'Power will turn off.'
    else
      this.modalText = 'Power will turn on.'
    this.isToggleChecked = !this.isToggleChecked;
  }

  createLineCharts() {
    this.createLineChart(this.lineChart1, "MyLineChart1", this.lineChartLabels1, this.lineChartCon1);
    this.createLineChart(this.lineChart2, "MyLineChart2", this.lineChartLabels1, [5, 4, 3, 2]);
    this.createLineChart(this.lineChart3, "MyLineChart3", this.lineChartLabels3, this.lineChartCon3);
    this.createLineChart(this.lineChart4, "MyLineChart4", this.lineChartLabels4, this.lineChartCon4);
    this.createLineChart(this.lineChart5, "MyLineChart5", this.lineChartLabels5, this.lineChartCon5);
    this.createLineChart(this.lineChart6, "MyLineChart6", this.lineChartLabels6, this.lineChartCon6);
  }

  createLineChart(chart: any, chartName: any, labels: any, chartData: any) {
    chart = new Chart(chartName, {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: labels,
        datasets: [
          {
            label: "Consumed",
            data: chartData,
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
  selectOption(option: string) {
    this.selectedOption = option;
  }
}
