// 버튼 클릭 시 실행될 함수
function showAlert() {
    window.location.href = '/webserv_tester.html';
}

// 모든 버튼에 클릭 이벤트 추가
const elements = document.getElementsByClassName("button");


for (let i = 0; i < elements.length; i++) {
    elements[i].addEventListener("click", showAlert);
}