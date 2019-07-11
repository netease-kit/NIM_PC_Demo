window.onload = () => {
  // 从 json 中读取 emoji 内容
  const emojiArray = emoji.emoji

  // 注册添加消息的接口
  NimCefWebInstance.register('addMsg', (params) => {
    const list = document.getElementById('chat-list-container');
    const msgAccount = params.account
    const msgNick = params.nickname
    const msgIsSelf = params.self
    const type = params.type;
    const isHistory = params.isHistory
    let msgContent = params.content
    
    if (msgContent.length === 0) {
      return
    }

    if (type === 0) { // 普通文本消息或带有表情的消息
      // regex emoji
      const regex = /\[.+?\]/gmi
      const tags = msgContent.match(regex)
      if (tags && typeof tags === 'object' && tags.length > 0) {
        tags.map(tag => {
          emojiArray.map(emoji => {
            if (emoji.tag === tag) {
              msgContent = msgContent.replace(tag, `<img class="emoji" src="../../res/emoji/${emoji.file}" />`)
            }
          })
        })
      }
    } else if (type === 1) { // 猜拳
      msgContent = msgContent.replace(msgContent, `<img src="../../resources/themes/default/session/${msgContent}" />`)
    } else if (type === 2) { // 机器人消息
      const robotMsg = JSON.parse(msgContent)
      const parser = new DOMParser();
      const xmlDoc = parser.parseFromString(robotMsg.content, 'text/xml');
      const root = xmlDoc.firstChild;
      let robotHTML = ''

      if (robotMsg.type === '01') {
        msgContent = robotMsg.content
      } else if (robotMsg.type === '02') {
        msgContent = `<img src="${robotMsg.content}" />`
      } else if (robotMsg.type === '11') {
        for (let i = 0; i < root.children.length; i++) {
          const content = root.children[i]
          for (let j = 0; j < content.children.length; j++) {
            if (content.children[j].tagName === 'text') {
              robotHTML += content.children[j].innerHTML
            } else if (content.children[j].tagName === 'link') {
              const text = content.children[j].children[0]
              const target = content.children[j].getAttribute('target')
              const type = content.children[j].getAttribute('type')
              const params = content.children[j].getAttribute('params')
              const style = content.children[j].getAttribute('style')
              robotHTML += `<a class="robot-link" robot-account="${msgAccount}" target="${target}" type-value="${type}" params="${params}">${text.innerHTML}</a>`
            }
          }
        }
        msgContent = robotHTML;
      }
    }

    // user
    const user = document.createElement('h5')
    user.setAttribute('class', `item-user`)
    user.setAttribute('id', msgAccount)
    user.innerText = msgNick
  
    // content
    const contentDiv = document.createElement('div')
    contentDiv.setAttribute('class', 'item-content')
    const arrow = document.createElement('div')
    arrow.setAttribute('class', 'arrow')
    const content = document.createElement('p')
    content.innerHTML = msgContent
    contentDiv.appendChild(arrow)
    contentDiv.appendChild(content)
  
    // container
    const container = document.createElement('div')
    container.setAttribute('class', `item item-${ msgIsSelf ? 'right' : 'left' }`)
    container.appendChild(user)
    container.appendChild(contentDiv)
    const wrapper = document.createElement('div')
    wrapper.setAttribute('class', ' wrapper')
    wrapper.appendChild(container)
  
    if (list.children.length > 1000) {
      list.removeChild(list.childNodes[0])
    }

    // append to list
    if (isHistory) {
      list.insertBefore(wrapper, list.childNodes[0])
      document.scrollingElement.scrollTop = document.scrollingElement.scrollHeight - document.lastScrollHeight
    } else {
      list.appendChild(wrapper)
      document.scrollingElement.scrollTop = document.scrollingElement.scrollHeight
    }
    
    if (type === 2) {
      const robotLinks = document.getElementsByClassName('robot-link')
      for (let i = 0; i < robotLinks.length; i++) {
        robotLinks[i].onclick = replyRobot
      }
    }
  })

  NimCefWebInstance.register('addNotify', (params) => {
    const isHistory = params.isHistory
    const content = params.content
    
    if (content.length === 0) {
      return
    }
    
    // notify content
    const span = document.createElement('span')
    span.innerText = params.content;

    // append to list
    const wrapper = document.createElement('div')
    const list = document.getElementById('chat-list-container');
    wrapper.appendChild(span)
    wrapper.setAttribute('class', 'notify-wrapper')

    if (isHistory) {
      list.insertBefore(wrapper, list.childNodes[0])
      document.scrollingElement.scrollTop = document.scrollingElement.scrollHeight - document.lastScrollHeight
    } else {
      list.appendChild(wrapper)
      document.scrollingElement.scrollTop = document.scrollingElement.scrollHeight
    }
  })

  // 添加滚动条监听事件
  window.addEventListener('scroll', (event) => {
    if (document.scrollingElement.scrollTop === 0) {
      document.lastScrollHeight = document.scrollingElement.scrollHeight
      NimCefWebInstance.call('getHistory', (error, result) => {
        if (error) {
          console.log(JSON.parse(result))
        }
      })
    }
  })
}

document.oncontextmenu = (event) => {
  if (window.event) {
    event = window.event
  }

  // 只在用户名称中弹出右键菜单
  if (event.target.className != 'item-user') {
    return false
  }

  NimCefWebInstance.call('getMemberInfo', { account: event.target.id }, (error, result) => {
    if (!error && result) {
      // 不能操作自己
      if (result.self) {
        return false;
      }

      // 只有管理员和创建者可以操作
      if (!result.hasPermission) {
        return false;
      }

      // 如果目标用户是创建者则不能操作
      if (result.isCreator) {
        return false
      }

      // 如果目标用户和自己都是管理员，则不能操作，直接返回
      if (result.isAdmin) {
        return false
      }

      const kick = document.getElementById('kick');
      kick.parentElement.style.display = 'block';
      kick.onclick = kickMember

      const mute = document.getElementById('mute')
      mute.parentElement.style.display = result.isMuted ? 'none' : 'block';
      mute.onclick = setMemberMute

      const unmute = document.getElementById('unmute')
      unmute.parentElement.style.display = result.isMuted ? 'block' : 'none';
      unmute.onclick = setMemberMute

      const black = document.getElementById('black')
      black.parentElement.style.display = result.inBlackList ? 'none' : 'block';
      black.onclick = setMemberBlack

      const cancelBlack = document.getElementById('cancel-black')
      cancelBlack.parentElement.style.display = result.inBlackList ? 'block' : 'none';
      cancelBlack.onclick = setMemberBlack

      const setAdmin = document.getElementById('set-admin').parentElement
      setAdmin.style.display = result.memberType === 2 ? 'none' : 'block';
      setAdmin.onclick = setMemberAdmin

      const cancelAdmin = document.getElementById('cancel-admin')
      cancelAdmin.parentElement.style.display = result.memberType === 2 ? 'block' : 'none';
      cancelAdmin.onclick = setMemberAdmin

      if (result.memberType !== 2) {
        const tempMute = document.getElementById('temp-mute')
        tempMute.parentElement.style.display = result.isTempMuted ? 'none' : 'block';
        tempMute.onclick = setMemberTempMute

        const unTempMute = document.getElementById('temp-unmute')
        unTempMute.parentElement.style.display = result.isTempMuted ? 'block' : 'none';
        unTempMute.onclick = setMemberTempMute
      }

      const menu = document.getElementById("menu")
      menu.style.visibility = "visible"
      menu.style.display = "block"
      const scrollTop = document.documentElement.scrollTop || document.body.scrollTop
      menu.style.left = event.clientX + 5 + "px"
      menu.style.top = event.clientY + scrollTop + "px"
    }
  })

  return false
}

document.onclick = () => {
  const menu = document.getElementById("menu")
  menu.style.visibility = "hidden"
  menu.style.display = "none"
  return false
}

const replyRobot = (event) => {
  const account = event.target.getAttribute('robot-account')
  const params = event.target.getAttribute('params')
  const target = event.target.getAttribute('target')
  const type = event.target.getAttribute('type-value')
  const linkText = event.target.innerText
  
  NimCefWebInstance.call('replyRobot', { account, params, target, type, linkText }, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}

const kickMember = (event) => {
  NimCefWebInstance.call('kickMember', {}, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}

const setMemberMute = (event) => {
  NimCefWebInstance.call('setMemberMute', { setMute: event.target.id === 'mute' }, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}

const setMemberBlack = (event) => {
  NimCefWebInstance.call('setMemberBlack', { setBlack: event.target.id === 'black' }, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}

const setMemberAdmin = (event) => {
  NimCefWebInstance.call('setMemberAdmin', { setAdmin: event.target.id === 'set-admin' }, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}

const setMemberTempMute = (event) => {
  NimCefWebInstance.call('setMemberTempMute', { setTempMute: event.target.id === 'temp-mute' }, (error, result) => {
    if (error) {
      console.log(result)
    }
  })
}
