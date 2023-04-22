import { Component, OnInit } from "@angular/core";
import { Chart } from "chart.js";
import { ChartConfiguration } from "chart.js";
import { ApiService } from "src/app/services/api.service";

export interface User {
  customerId: number;
  firstName: string;
  lastName: string;
  email: string;
}

@Component({
  selector: "app-dashboard",
  templateUrl: "./dashboard.component.html",
  styleUrls: ["./dashboard.component.scss"],
})
export class DashboardComponent implements OnInit {
  isToggleChecked: boolean = true;
  modalText: string = "Power will turn off.";
  user: User;
  selectedOption: string = "Last 30 Minutes";
  lineChart1: any;
  lineChart2: any;
  lineChart3: any;
  lineChart4: any;
  lineChart5: any;
  lineChart6: any;
  lineChartCon1: number[];
  lineChartLabels1: string[];
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
  serviceParams: any = {
    "Last 30 Minutes": { timeType: 1, time: 30 },
    "Last 1 Hour": { timeType: 2, time: 1 },
    "Last 3 Hours": { timeType: 2, time: 3 },
    "Last 6 Hours": { timeType: 2, time: 6 },
    "Last 12 Hours": { timeType: 2, time: 12 },
    "Last 24 Hours": { timeType: 2, time: 24 },
  };

  constructor(public _api: ApiService) {
    this._api.GetUser().subscribe((res) => {
      this.user = res;
    });
  }

  ngOnInit(): void {
    this._api.RecentMeasurementPost(1, 1, 30).subscribe((res)=>{
      this.lineChartCon1 = res.map((item: any) => item.value);
      this.lineChartLabels1 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.createLineCharts();
    });
  }

  clicked() {
    if (!this.isToggleChecked) this.modalText = "Power will turn off.";
    else this.modalText = "Power will turn on.";
    this.isToggleChecked = !this.isToggleChecked;
  }

  createLineCharts() {
    this.lineChart1 = this.createLineChart(
      "MyLineChart1",
      this.lineChartLabels1,
      this.lineChartCon1
    );
    this.lineChart2 = this.createLineChart(
      "MyLineChart2",
      this.lineChartLabels2,
      this.lineChartCon2
    );
    this.lineChart3 = this.createLineChart(
      "MyLineChart3",
      this.lineChartLabels3,
      this.lineChartCon3
    );
    this.lineChart4 = this.createLineChart(
      "MyLineChart4",
      this.lineChartLabels4,
      this.lineChartCon4
    );
    this.lineChart5 = this.createLineChart(
      "MyLineChart5",
      this.lineChartLabels5,
      this.lineChartCon5
    );
    this.lineChart6 = this.createLineChart(
      "MyLineChart6",
      this.lineChartLabels6,
      this.lineChartCon6
    );
  }

  createLineChart(chartName: any, labels: any, chartData: any) {
    return new Chart(chartName, {
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

  send(timeType: number, time: number) {
    this._api.RecentMeasurementPost(1, timeType, time).subscribe((res) => {
      this.lineChartCon1 = res.map((item: any) => item.value);
      this.lineChartLabels1 = res.map((item: any) => item.time.slice(11, 19));
      this.lineChart1.data.datasets[0].data = this.lineChartCon1;
      this.lineChart1.data.labels = this.lineChartLabels1;
      this.lineChart1.update();
    });
  }
}

// MeasurementType: any = {
//   Voltage: 1,
//   Current: 2,
//   Power: 3,
//   Frequency: 4,
//   "Power Factor": 5,
//   Energy: 6,
// };
