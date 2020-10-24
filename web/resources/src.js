function manage_response(data)
{
	if (data.includes("ERROR:"))
	{
		alert(data.substring(6));
	} else
	{
		alert(data);
	}
}

function upload()
{
	data = new FormData()
	
	data.append("map1", document.getElementById("first_map").files[0])
	data.append("map2", document.getElementById("second_map").files[0])
	
	$.ajax({
		url: 'upload',  
		type: 'POST',
		data: data,
		success:function(data){
			manage_response(data)
		},
		cache: false,
		contentType: false,
		processData: false
	});
};

