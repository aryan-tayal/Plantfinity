let soilhumidityChartValues =
  JSON.parse(window.localStorage.getItem("soilhumidity")) || [];
let humidityChartValues =
  JSON.parse(window.localStorage.getItem("humidity")) || [];
let tempChartValues =
  JSON.parse(window.localStorage.getItem("temperature")) || [];
let growthChartValues = JSON.parse(window.localStorage.getItem("growth")) || [];

const alertContainer = document.querySelector("#alert-container");
const button = document.querySelector("#clearBtn");

const getCurrentTime = () => {
  return new Date(Date.now() + 330 * 60000).getTime();
};

let chartS = new Highcharts.Chart({
  chart: { renderTo: "chart-soil-humidity" },
  title: { text: "Soil Humidity" },
  series: [
    {
      showInLegend: false,
      data: soilhumidityChartValues,
    },
  ],
  plotOptions: {
    line: { animation: true, dataLabels: { enabled: true } },
    series: { color: "#03f3a0" },
  },
  xAxis: { type: "datetime", dateTimeLabelFormats: { second: "%H:%M" } },
  yAxis: {
    title: { text: "Soil Humidity (%)" },
  },
  credits: { enabled: false },
});
setInterval(() => {}, 20000);

let chartT = new Highcharts.Chart({
  chart: { renderTo: "chart-temperature" },
  title: { text: "Ambient Temperature" },
  series: [
    {
      showInLegend: false,
      data: tempChartValues,
    },
  ],
  plotOptions: {
    line: { animation: true, dataLabels: { enabled: true } },
    series: { color: "#059e8a" },
  },
  xAxis: { type: "datetime", dateTimeLabelFormats: { second: "%H:%M" } },
  yAxis: {
    title: { text: "Temperature (Celsius)" },
  },
  credits: { enabled: false },
});

let chartH = new Highcharts.Chart({
  chart: { renderTo: "chart-humidity" },
  title: { text: "Ambient Humidity" },
  series: [
    {
      showInLegend: false,
      data: humidityChartValues,
    },
  ],
  plotOptions: {
    line: { animation: false, dataLabels: { enabled: true } },
  },
  xAxis: {
    type: "datetime",
    dateTimeLabelFormats: { second: "%H:%M" },
  },
  yAxis: {
    title: { text: "Humidity (%)" },
  },
  credits: { enabled: false },
});

let chartG = new Highcharts.Chart({
  chart: { renderTo: "chart-growth" },
  title: { text: "Plant Height" },
  series: [
    {
      showInLegend: false,
      data: growthChartValues,
    },
  ],
  plotOptions: {
    line: { animation: false, dataLabels: { enabled: true } },
    series: { color: "#18009c" },
  },
  xAxis: {
    type: "datetime",
    dateTimeLabelFormats: { second: "%H:%M" },
  },
  yAxis: {
    title: { text: "Plant Height (cm)" },
  },
  credits: { enabled: false },
});
const getData = {
  soilhumidity: async () => {
    const res = await axios.get("/soilhumidity");
    let x = getCurrentTime(),
      y = parseFloat(res.data);
    soilhumidityChartValues.push({ x, y });
    window.localStorage.setItem(
      "soilhumidity",
      JSON.stringify(soilhumidityChartValues)
    );
    if (chartS.series[0].data.length > 10) {
      chartS.series[0].addPoint([x, y], true, true, true);
    } else {
      chartS.series[0].addPoint([x, y], true, false, true);
    }
  },
  humidity: async () => {
    const res = await axios.get("/humidity");
    let x = getCurrentTime(),
      y = parseFloat(res.data);
    humidityChartValues.push({ x, y });
    window.localStorage.setItem(
      "humidity",
      JSON.stringify(humidityChartValues)
    );
    if (chartH.series[0].data.length > 10) {
      chartH.series[0].addPoint([x, y], true, true, true);
    } else {
      chartH.series[0].addPoint([x, y], true, false, true);
    }
  },
  temp: async () => {
    const res = await axios.get("/temperature");
    let x = getCurrentTime(),
      y = parseFloat(res.data);
    tempChartValues.push({ x, y });
    window.localStorage.setItem("temperature", JSON.stringify(tempChartValues));
    if (chartT.series[0].data.length > 10) {
      chartT.series[0].addPoint([x, y], true, true, true);
    } else {
      chartT.series[0].addPoint([x, y], true, false, true);
    }
  },
  growth: async () => {
    const res = await axios.get("/growth");
    let x = getCurrentTime(),
      y = parseFloat(res.data);
    growthChartValues.push({ x, y });
    window.localStorage.setItem("growth", JSON.stringify(growthChartValues));
    if (chartG.series[0].data.length > 10) {
      chartG.series[0].addPoint([x, y], true, true, true);
    } else {
      chartG.series[0].addPoint([x, y], true, false, true);
    }
  },
  err: async () => {
    const res = await axios.get("/err");
    data = res.data.split(",");
    alertContainer.innerHTML = "";
    data.forEach((d) => {
      if (d.length) {
        let alert = document.createElement("div");
        alert.classList.add("alert");
        alert.innerHTML = `<p class="mb-0">${d}</p>`;
        alert.classList.add("alert");
        if (d == "All Good") {
          alert.classList.add("alert-success");
        } else {
          alert.classList.add("alert-danger");
        }
        alertContainer.appendChild(alert);
      }
    });
  },
};

setInterval(getData.soilhumidity, 20000);
setInterval(getData.temp, 20000);
setInterval(getData.humidity, 20000);
setInterval(getData.growth, 20000);
setInterval(getData.err, 20000);

const clearData = (criteria) => {
  window.localStorage.removeItem(criteria);
  location.reload();
};

button.addEventListener("click", () => {
  window.localStorage.clear();
  soilhumidityChartValues = [];
  humidityChartValues = [];
  growthChartValues = [];
  tempChartValues = [];
  location.reload();
});
