const fileInput = document.getElementById("myfile");
const fileInfo = document.getElementById("fileInfo")
var outfile;

fileInput.addEventListner("change", (event) => {
    const file = event.target.files[0];

    if (!file) {
        fileInfo.textContent = "No file selected";
        return;
    }

})