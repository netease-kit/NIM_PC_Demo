window.onload = () => {
  const chatroomManager = NimCefWebInstance.chatroom

  // 遍历聊天室列表数据插入到界面中
  chatroomManager.onChatroomListChanged((chatroom_list) => {
    const container = $('#chatroom-list-container')
    let rowDiv
    chatroom_list.map((chatroom, i) => {
      if (i % 4 === 0) {
        rowDiv = document.createElement('div')
        rowDiv.setAttribute('class', 'row')
        container.append(rowDiv)
      }
      
      // col div
      const colDiv = document.createElement('div')
      colDiv.setAttribute('class', 'col-xs-3 room-item')
      colDiv.setAttribute('room-id', chatroom.chatroomId)
      rowDiv.appendChild(colDiv)
      // 给每个房间的 div 增加单击事件
      colDiv.addEventListener('click', () => {
        chatroomManager.showChatroomById(chatroom.chatroomId)
      }, true)
  
      // background image
      const image = document.createElement('img')
      image.setAttribute('class', 'chatroom-image')
      image.setAttribute('src', 'default_image.png')
      colDiv.appendChild(image)
  
      // room name
      const title = document.createElement('p')
      title.setAttribute('class', 'room-name')
      const roomname = document.createTextNode(chatroom.chatroomName)
      title.appendChild(roomname)
      colDiv.appendChild(title)
  
      // number of online
      const countSpan = document.createElement('span')
      const count = document.createTextNode('在线人数：')
      countSpan.appendChild(count)
      colDiv.appendChild(countSpan)
  
      const numberSpan = document.createElement('span')
      numberSpan.setAttribute('class', 'room-name')
      const numberOfOnline = document.createTextNode(chatroom.numberOfOnline.toString())
      numberSpan.appendChild(numberOfOnline)
      colDiv.appendChild(numberSpan)
    })
  })
}
